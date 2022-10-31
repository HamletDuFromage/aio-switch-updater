#include "download.hpp"

#include <curl/curl.h>
#include <math.h>
#include <mbedtls/base64.h>
#include <switch.h>
#include <time.h>

#include <algorithm>
#include <chrono>
#include <regex>
#include <string>
#include <thread>

#include "fs.hpp"
#include "progress_event.hpp"
#include "utils.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

constexpr const char API_AGENT[] = "HamletDuFromage";
constexpr int _1MiB = 0x100000;

using json = nlohmann::ordered_json;

namespace download {

    namespace {

        std::chrono::_V2::steady_clock::time_point time_old;
        double dlold;

        typedef struct
        {
            char* memory;
            size_t size;
        } MemoryStruct_t;

        typedef struct
        {
            u_int8_t* data;
            size_t data_size;
            u_int64_t offset;
            FILE* out;
            Aes128CtrContext* aes;
        } ntwrk_struct_t;

        static size_t WriteMemoryCallback(void* contents, size_t size, size_t num_files, void* userp)
        {
            if (ProgressEvent::instance().getInterupt()) {
                return 0;
            }
            ntwrk_struct_t* data_struct = (ntwrk_struct_t*)userp;
            size_t realsize = size * num_files;

            if (realsize + data_struct->offset >= data_struct->data_size) {
                fwrite(data_struct->data, data_struct->offset, 1, data_struct->out);
                data_struct->offset = 0;
            }

            if (data_struct->aes)
                aes128CtrCrypt(data_struct->aes, &data_struct->data[data_struct->offset], contents, realsize);
            else
                memcpy(&data_struct->data[data_struct->offset], contents, realsize);

            data_struct->offset += realsize;
            data_struct->data[data_struct->offset] = 0;
            return realsize;
        }

        int download_progress(void* p, double dltotal, double dlnow, double ultotal, double ulnow)
        {
            if (dltotal <= 0.0) return 0;

            double fractionDownloaded = dlnow / dltotal;
            int counter = (int)(fractionDownloaded * ProgressEvent::instance().getMax());  //20 is the number of increments
            ProgressEvent::instance().setStep(std::min(ProgressEvent::instance().getMax() - 1, counter));
            ProgressEvent::instance().setNow(dlnow);
            ProgressEvent::instance().setTotalCount(dltotal);
            auto time_now = std::chrono::steady_clock::now();
            double elasped_time = ((std::chrono::duration<double>)(time_now - time_old)).count();
            if (elasped_time > 1.2f) {
                ProgressEvent::instance().setSpeed((dlnow - dlold) / elasped_time);
                dlold = dlnow;
                time_old = time_now;
            }
            return 0;
        }

        struct MemoryStruct
        {
            char* memory;
            size_t size;
        };

        static size_t WriteMemoryCallback2(void* contents, size_t size, size_t nmemb, void* userp)
        {
            size_t realsize = size * nmemb;
            struct MemoryStruct* mem = (struct MemoryStruct*)userp;

            char* ptr = static_cast<char*>(realloc(mem->memory, mem->size + realsize + 1));
            if (ptr == NULL) {
                /* out of memory! */
                return 0;
            }

            mem->memory = ptr;
            memcpy(&(mem->memory[mem->size]), contents, realsize);
            mem->size += realsize;
            mem->memory[mem->size] = 0;

            return realsize;
        }

        bool checkSize(CURL* curl, const std::string& url)
        {
            curl_off_t dl;
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_USERAGENT, API_AGENT);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
            curl_easy_perform(curl);
            auto res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &dl);
            if (!res) {
                s64 freeStorage;
                if (R_SUCCEEDED(fs::getFreeStorageSD(freeStorage)) && dl * 1.1 > freeStorage) {
                    return false;
                }
            }
            return true;
        }

        std::string mega_id(std::string url)
        {
            auto len = url.length();

            if (len < 52)
                throw std::invalid_argument("Invalid URL.");

            bool old_link = url.find("#!") < len;

            /* Start from the last '/' and add 2 characters if old link format starting with '#!' */
            auto init_pos = url.find_last_of('/') + (old_link ? 2 : 0) + 1;

            /* End with the last '#' or "!" if its the old link format */
            auto end_pos = url.find_last_of(old_link ? '!' : '#');

            /* Finally crop the url */
            std::string id = url.substr(init_pos, end_pos - init_pos);

            if (id.length() != 8)
                throw std::invalid_argument("Invalid URL ID.");

            return id;
        }

        std::string mega_url(std::string url)
        {
            std::string id = mega_id(url);

            json request = json::array({{
                {"a", "g"},
                {"g", 1},
                {"p", id},
            }});

            std::string body = request.dump();
            std::string output;

            auto curl = curl_easy_init();
            curl_easy_setopt(curl, CURLOPT_URL, "https://g.api.mega.co.nz/cs");
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, API_AGENT);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(
                curl,
                CURLOPT_WRITEFUNCTION,
                +[](void* buffer, size_t size, size_t nmemb, void* userp) -> size_t {
                    std::string* output = reinterpret_cast<std::string*>(userp);
                    size_t actual_size = size * nmemb;
                    output->append(reinterpret_cast<char*>(buffer), actual_size);
                    return actual_size;
                });

            curl_easy_perform(curl);

            json response = json::parse(output);

            curl_easy_cleanup(curl);

            s64 freeStorage;
            s64 fileSize = response[0]["s"];
            if (R_SUCCEEDED(fs::getFreeStorageSD(freeStorage)) && fileSize * 1.1 > freeStorage)
                return "";

            return response[0]["g"];
        }

        std::string mega_node_key(std::string url)
        {
            /* Check if old link format */
            auto len = url.length();

            if (len < 52)
                throw std::invalid_argument("Invalid URL.");

            bool old_link = url.find("#!") < len;

            /* End with the last '#' or "!" if its the old link format */
            auto end_pos = url.find_last_of(old_link ? '!' : '#') + 1;

            /* Crop the URL to get the file key */
            std::string key = url.substr(end_pos, len - end_pos);

            /* Replace URL characters with B64 characters */
            std::replace(key.begin(), key.end(), '_', '/');
            std::replace(key.begin(), key.end(), '-', '+');

            /* Add padding */
            auto key_len = key.length();
            unsigned pad = 4 - key_len % 4;
            key.append(pad, '=');

            /* The encoded key should have 44 characters to produce a 32 byte node key */
            if (key.length() != 44)
                throw std::invalid_argument("Invalid URL key.");

            std::string decoded(key.size() * 3 / 4, 0);
            size_t olen = 0;

            mbedtls_base64_decode(
                reinterpret_cast<unsigned char*>(decoded.data()),
                decoded.size(),
                &olen,
                reinterpret_cast<const unsigned char*>(key.c_str()),
                key.size());

            /**
             * The encoded base64 is (usually?) 43 characters long. With padding it goes
             * to 44. When we calculate the decoded size, we need to allocate 33 bytes.
             * But the last encoded character is padding, and combined with the last 
             * valid character, it should produce a 32 byte node key.
             */
            decoded.resize(olen);
            if (decoded.size() != 32)
                throw std::invalid_argument("Invalid node key.");

            return decoded;
        }

        std::string mega_key(const std::string& node_key)
        {
            std::string key(16, 0);

            reinterpret_cast<uint64_t*>(key.data())[0] =
                reinterpret_cast<const uint64_t*>(node_key.data())[0] ^
                reinterpret_cast<const uint64_t*>(node_key.data())[2];
            reinterpret_cast<uint64_t*>(key.data())[1] =
                reinterpret_cast<const uint64_t*>(node_key.data())[1] ^
                reinterpret_cast<const uint64_t*>(node_key.data())[3];

            return key;
        }

        std::string mega_iv(const std::string& node_key)
        {
            std::string iv(16, 0);
            reinterpret_cast<uint64_t*>(iv.data())[0] =
                reinterpret_cast<const uint64_t*>(node_key.data())[2];

            return iv;
        }
    }  // namespace

    long downloadFile(const std::string& url, const std::string& output, int api)
    {
        std::vector<std::uint8_t> dummy;
        return downloadFile(url, dummy, output, api);
    }

    long downloadFile(const std::string& url, std::vector<std::uint8_t>& res, const std::string& output, int api)
    {
        const char* out = output.c_str();
        CURL* curl = curl_easy_init();
        ntwrk_struct_t chunk = {0};
        long status_code;
        time_old = std::chrono::steady_clock::now();
        dlold = 0.0f;
        bool can_download = true;
        bool is_mega = (url.find("mega.nz") != std::string::npos);
        std::string real_url = is_mega ? mega_url(url) : url;

        if (is_mega) {
            std::string node_key = mega_node_key(url);
            std::string key = mega_key(node_key);
            std::string iv = mega_iv(node_key);

            chunk.aes = static_cast<Aes128CtrContext*>(malloc(sizeof(Aes128CtrContext)));
            aes128CtrContextCreate(chunk.aes, key.c_str(), iv.c_str());
        }

        if (curl) {
            FILE* fp = fopen(out, "wb");
            if (fp || *out == 0) {
                chunk.data = static_cast<u_int8_t*>(malloc(_1MiB));
                chunk.data_size = _1MiB;
                chunk.out = fp;

                if (*out != 0) {
                    can_download = is_mega ? !real_url.empty() : checkSize(curl, url);
                }

                if (can_download) {
                    curl_easy_setopt(curl, CURLOPT_URL, real_url.c_str());
                    curl_easy_setopt(curl, CURLOPT_USERAGENT, API_AGENT);
                    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
                    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
                    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
                    curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
                    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
                    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

                    if (api == OFF) {
                        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
                        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, download_progress);
                    }
                    curl_easy_perform(curl);
                    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);

                    if (fp && chunk.offset && can_download)
                        fwrite(chunk.data, 1, chunk.offset, fp);

                    curl_easy_cleanup(curl);
                    ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
                }
            }
        }

        fclose(chunk.out);
        if (!can_download) {
            brls::Application::crash("menus/errors/insufficient_storage"_i18n);
            std::this_thread::sleep_for(std::chrono::microseconds(2000000));
            brls::Application::quit();
            res = {};
        }

        if (*out == 0) {
            res.assign(chunk.data, chunk.data + chunk.offset);
        }

        free(chunk.data);
        free(chunk.aes);

        return status_code;
    }

    std::string fetchTitle(const std::string& url)
    {
        CURL* curl_handle;
        struct MemoryStruct chunk;

        chunk.memory = static_cast<char*>(malloc(1)); /* will be grown as needed by the realloc above */
        chunk.size = 0;                               /* no data at this point */

        curl_global_init(CURL_GLOBAL_ALL);
        curl_handle = curl_easy_init();
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback2);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, API_AGENT);

        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_perform(curl_handle);

        /* check for errors */
        std::string ver = "-1";
        std::string s = std::string(chunk.memory);
        std::regex rgx("<title>.+</title>");
        std::smatch match;
        if (std::regex_search(s, match, rgx)) {
            //ver = std::stoi(match[0]);
            //std::cout << match[0].str().substr(match[0].str().find(" ") + 1, 6) << std::endl;
            ver = match[0].str().substr(match[0].str().find(" ") + 1, 5);
        }
        curl_easy_cleanup(curl_handle);
        free(chunk.memory);

        curl_global_cleanup();

        return ver;
    }

    long downloadPage(const std::string& url, std::string& res, const std::vector<std::string>& headers, const std::string& body)
    {
        CURL* curl_handle;
        struct MemoryStruct chunk;
        struct curl_slist* list = NULL;
        long status_code;

        chunk.memory = static_cast<char*>(malloc(1)); /* will be grown as needed by the realloc above */
        chunk.size = 0;                               /* no data at this point */

        curl_global_init(CURL_GLOBAL_ALL);
        curl_handle = curl_easy_init();
        curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
        if (!headers.empty()) {
            for (auto& h : headers) {
                list = curl_slist_append(list, h.c_str());
            }
            curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, list);
        }
        if (body != "") {
            curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, body.c_str());
        }

        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback2);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, API_AGENT);

        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_perform(curl_handle);
        curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &status_code);
        curl_easy_cleanup(curl_handle);
        res = std::string(chunk.memory);
        free(chunk.memory);

        curl_global_cleanup();
        return status_code;
    }

    long getRequest(const std::string& url, nlohmann::ordered_json& res, const std::vector<std::string>& headers, const std::string& body)
    {
        std::string request;
        long status_code = downloadPage(url, request, headers, body);

        if (json::accept(request))
            res = nlohmann::ordered_json::parse(request);
        else
            res = nlohmann::ordered_json::object();

        return status_code;
    }

    std::vector<std::pair<std::string, std::string>> getLinks(const std::string& url)
    {
        nlohmann::ordered_json request;
        getRequest(url, request);

        std::vector<std::pair<std::string, std::string>> res;
        for (auto it = request.begin(); it != request.end(); ++it) {
            res.push_back(std::make_pair(it.key(), it.value()));
        }
        return res;
    }

    std::vector<std::pair<std::string, std::string>> getLinksFromJson(const nlohmann::ordered_json& json_object)
    {
        std::vector<std::pair<std::string, std::string>> res;
        for (auto it = json_object.begin(); it != json_object.end(); ++it) {
            res.push_back(std::make_pair(it.key(), it.value()));
        }
        return res;
    }

}  // namespace download