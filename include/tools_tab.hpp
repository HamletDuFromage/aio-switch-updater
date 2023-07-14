#pragma once

#include <borealis.hpp>
#include <json.hpp>
//#include "ntcp.hpp"

class ToolsTab : public brls::List
{
private:
    brls::StagedAppletFrame* stagedFrame;

public:
    ToolsTab(const std::string& tag, const nlohmann::ordered_json& payloads, bool erista = true, const nlohmann::ordered_json& hideStatus = {});
};