#include <filesystem>
#include <string>

#include <switch.h>

#define PATH        "/switch/aio-switch-updater/"
#define FULL_PATH   "/switch/aio-switch-updater/aio-switch-updater.nro"
#define CONFIG_PATH "/config/aio-switch-updater/switch/aio-switch-updater/aio-switch-updater.nro"
#define PREFIX      "/switch/aio-switch-updater/aio-switch-updater-v"
#define FORWARDER_PATH      "/config/aio-switch-updater/aiosu-forwarder.nro"


int main(int argc, char* argv[])

{
    std::filesystem::create_directory(PATH);
    for (const auto & entry : std::filesystem::directory_iterator(PATH)){
        if(entry.path().string().find(PREFIX) != std::string::npos) {
            std::filesystem::remove(entry.path().string());
            std::filesystem::remove(entry.path().string() + ".star");
        }
    }

    if(std::filesystem::exists(CONFIG_PATH)){
        std::filesystem::create_directory(PATH);
        std::filesystem::remove(FULL_PATH);
        std::filesystem::rename(CONFIG_PATH, FULL_PATH);
        std::filesystem::remove_all("/config/aio-switch-updater/switch/aio-switch-updater/");
        rmdir("/config/aio-switch-updater/switch/aio-switch-updater/");
        rmdir("/config/aio-switch-updater/switch/");
    }

    std::filesystem::remove(FORWARDER_PATH);

    envSetNextLoad(FULL_PATH, ("\"" + std::string(FULL_PATH) + "\"").c_str());
    return 0;
}
