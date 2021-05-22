#pragma once

constexpr int ON =      1;
constexpr int OFF =     0;

#include <json.hpp>

namespace download {
    
    std::vector<std::uint8_t> downloadFile(const std::string& url, const char* output = "", int api = OFF);
    std::vector<std::pair<std::string, std::string>> getLinks(const std::string& url);
    std::string fetchTitle(const std::string& url);
    std::string downloadPage(const std::string& url, std::vector<std::string> headers = {}, std::string body = "");
    std::vector<std::uint8_t> downloadPageBinary(const std::string& url, std::vector<std::string> headers = {}, std::string body = "");
    nlohmann::ordered_json getRequest(const std::string& url, std::vector<std::string> headers = {}, std::string body = "");

}