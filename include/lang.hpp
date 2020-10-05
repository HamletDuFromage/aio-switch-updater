#pragma once

#include <string>
#include <json.hpp>
#include <switch.h>
namespace lang {

enum class Language {
    English,
    Chinese,
   /* French,
    Dutch,
    Italian,
    German,
    Spanish,*/
    Default,
};

const nlohmann::json &get_json();

Language get_current_language();
Result set_language(Language lang);
Result initialize_to_system_language();

std::string get_string(std::string key, const nlohmann::json &json = get_json());

namespace literals {

inline std::string operator ""_lang(const char *key, size_t size) {
    return get_string(std::string(key, size));
}

} // namespace literals

}; // namespace lang
