#include "download.hpp"
#include "utils.hpp"

#include <algorithm>


#define API_AGENT           "HamletDuFromage"
#define _1MiB   0x100000

using json = nlohmann::json;

typedef struct
{
    char *memory;
    size_t size;
} MemoryStruct_t;

typedef struct
{
    u_int8_t *data;
    size_t data_size;
    u_int64_t offset;
    FILE *out;
} ntwrk_struct_t;

static size_t WriteMemoryCallback(void *contents, size_t size, size_t num_files, void *userp)
{
    ntwrk_struct_t *data_struct = (ntwrk_struct_t *)userp;
    size_t realsize = size * num_files;

    if (realsize + data_struct->offset >= data_struct->data_size)
    {
        fwrite(data_struct->data, data_struct->offset, 1, data_struct->out);
        data_struct->offset = 0;
    }

    memcpy(&data_struct->data[data_struct->offset], contents, realsize);
    data_struct->offset += realsize;
    data_struct->data[data_struct->offset] = 0;
    return realsize;
}

int download_progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow)
{
    if (dltotal <= 0.0) return 0;

    double fractionDownloaded = dlnow / dltotal;
    int counter = (int) (fractionDownloaded * ProgressEvent::instance().getMax()); //20 is the number of increments
    ProgressEvent::instance().setStep(std::min(ProgressEvent::instance().getMax() - 1, counter));
    return 0;
}

void downloadFile(const char *url, const char *output, int api)
{
    ProgressEvent::instance().reset();
    CURL *curl = curl_easy_init();
    if (curl)
    {
        FILE *fp = fopen(output, "wb");
        if (fp)
        {
            printf("\n");

            ntwrk_struct_t chunk = {0};
            chunk.data = static_cast<u_int8_t *>(malloc(_1MiB));
            chunk.data_size = _1MiB;
            chunk.out = fp;

            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, API_AGENT);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

            // write calls
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

            if (api == OFF)
            {
                curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
                //curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &prog);
                curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, download_progress);
            }
            curl_easy_perform(curl);
            if (chunk.offset)
              fwrite(chunk.data, 1, chunk.offset, fp);

            curl_easy_cleanup(curl);
            free(chunk.data);
            fclose(chunk.out);

            ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
        }
    }
}

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback2(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = static_cast<char *>(realloc(mem->memory, mem->size + realsize + 1));
    if(ptr == NULL) {
        /* out of memory! */ 
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}


std::tuple<std::vector<std::string>, std::vector<std::string>> fetchLinks(const char *url){
    CURL *curl_handle;
    struct MemoryStruct chunk;

    chunk.memory = static_cast<char *>(malloc(1));  /* will be grown as needed by the realloc above */ 
    chunk.size = 0;    /* no data at this point */ 

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback2);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, API_AGENT);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_perform(curl_handle);
 
    std::tuple <std::vector<std::string>, std::vector<std::string>> links;

    std::string s = std::string(chunk.memory);
    //<a href=.*?> links
    //>(?:(?!>).)*?</a> titles
    auto titles = htmlProcess(s, std::regex(">(?:(?!>).)*?</a>"));
    std::transform(titles.begin(), titles.end(), titles.begin(), 
                    [](std::string s) -> std::string {return s.substr(1, s.size() - 1 - 4);});
    auto targets = htmlProcess(s, std::regex("<a href=.*?>"));
    std::transform(targets.begin(), targets.end(), targets.begin(), 
                    [](std::string s) -> std::string {return s.substr(8, s.size() - 8 - 1);});
    links = std::make_tuple(titles, targets);

    curl_easy_cleanup(curl_handle);
    free(chunk.memory);
    curl_global_cleanup();
    return links;
}

std::string fetchTitle(const char *url){
    CURL *curl_handle; 
    struct MemoryStruct chunk;
 
    chunk.memory = static_cast<char *>(malloc(1));  /* will be grown as needed by the realloc above */ 
    chunk.size = 0;    /* no data at this point */ 
 
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback2);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, API_AGENT);

    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_perform(curl_handle);
 
  /* check for errors */ 
    std::string ver = "-1";
    std::string s = std::string(chunk.memory);
    std::regex rgx("<title>.+</title>");
    std::smatch match;
    if (std::regex_search(s, match, rgx)){
        //ver = std::stoi(match[0]);
        //std::cout << match[0].str().substr(match[0].str().find(" ") + 1, 6) << std::endl;
        ver = match[0].str().substr(match[0].str().find(" ") + 1, 5);
    }
    curl_easy_cleanup(curl_handle);
    free(chunk.memory);
 
    curl_global_cleanup();
 
    return ver;
}

std::string downloadPage(const char* url, std::vector<std::string> headers, std::string body){
    std::string res;
    CURL *curl_handle; 
    struct MemoryStruct chunk;
    struct curl_slist *list = NULL;
 
    chunk.memory = static_cast<char *>(malloc(1));  /* will be grown as needed by the realloc above */ 
    chunk.size = 0;    /* no data at this point */ 
 
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    if(!headers.empty()){
        for (auto& h : headers){
            list = curl_slist_append(list, h.c_str());
        }
        curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, list);
    }
    if(body != "") {
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, body.c_str());
    }

    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback2);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, API_AGENT);

    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_perform(curl_handle);
    curl_easy_cleanup(curl_handle);
    res = std::string(chunk.memory);
    free(chunk.memory);
 
    curl_global_cleanup();
    return res;
}

json getRequest(std::string url, std::vector<std::string> headers, std::string body) {
    std::string request;
    request = downloadPage(url.c_str(), headers, body);

    json res = {};
    bool valid = json::accept(request);
    if(valid)       return json::parse(request);
    else            return json::object();
}