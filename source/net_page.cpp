#include "net_page.hpp"

#include <arpa/inet.h>
#include <switch.h>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <thread>
#include <json.hpp>

#include "constants.hpp"
#include "fs.hpp"
#include "main_frame.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
using json = nlohmann::ordered_json;

NetPage::NetPage() : AppletFrame(true, true)
{
    this->setTitle("menus/net/title"_i18n);
    list = new brls::List();

    nifmInitialize(NifmServiceType_User);
    NifmNetworkProfileData profile;
    nifmGetCurrentNetworkProfile(&profile);
    nifmExit();

    int uuid = std::accumulate(profile.uuid.uuid, profile.uuid.uuid + 16, 0);

    std::string labelText;

    if (!uuid || !profile.ip_setting_data.mtu) {
        labelText = "Please connect to internet to use this feature.";
        label = new brls::Label(brls::LabelStyle::DESCRIPTION, labelText, true);
        list->addView(label);
        cancel = new brls::ListItem("menus/common/go_back"_i18n);
        cancel->getClickEvent()->subscribe([](brls::View* view) { brls::Application::pushView(new MainFrame()); });
        list->addView(cancel);
    }
    else {
        if (profile.ip_setting_data.ip_address_setting.is_automatic) {
            labelText = "IP Adress: Automatic";
        }
        else {
            labelText = fmt::format(
                "IP Adress: {}\nSubnet Mask: {}\nGateway: {}",
                ipToString(profile.ip_setting_data.ip_address_setting.current_addr.addr),
                ipToString(profile.ip_setting_data.ip_address_setting.subnet_mask.addr),
                ipToString(profile.ip_setting_data.ip_address_setting.gateway.addr));
        }
        labelText = fmt::format("{}\nLocal IP addr: {}\nMTU: {}", labelText, std::string(inet_ntoa({(in_addr_t)gethostid()})), std::to_string(unsigned(profile.ip_setting_data.mtu)));

        if (profile.ip_setting_data.dns_setting.is_automatic) {
            labelText = fmt::format("{}\nDNS: Automatic", labelText);
        }
        else {
            labelText = fmt::format(
                "{}\nPrimary DNS: {}\nSecondary DNS: {}",
                labelText,
                ipToString(profile.ip_setting_data.dns_setting.primary_dns_server.addr),
                ipToString(profile.ip_setting_data.dns_setting.secondary_dns_server.addr));
        }

        label = new brls::Label(brls::LabelStyle::DESCRIPTION, labelText, true);
        list->addView(label);

        //ip_addr
        //subnet_mask
        //gateway
        //dns1
        //dns2
        //mtu
        //ip_auto
        //dns_auto

        json profiles = fs::parseJsonFile(INTERNET_JSON);
        if (profiles.empty()) {
            profiles = json::array();
        }

        profiles.push_back(
            json::object({{"name", "lan-play"},
                          {"ip_addr", fmt::format("10.13.{}.{}", std::rand() % 256, std::rand() % 253 + 2)},
                          {"subnet_mask", "255.255.0.0"},
                          {"gateway", "10.13.37.1"}}));

        profiles.push_back(
            json::object({{"name", "Automatic IP Address"},
                          {"ip_auto", true}}));

        profiles.push_back(
            json::object({{"name", "Automatic DNS"},
                          {"dns_auto", true}}));

        profiles.push_back(
            json::object({{"name", "90DNS (Europe)"},
                          {"dns1", "163.172.141.219"},
                          {"dns2", "207.246.121.77"}}));

        profiles.push_back(
            json::object({{"name", "90DNS (USA)"},
                          {"dns1", "207.246.121.77"},
                          {"dns2", "163.172.141.219"}}));

        profiles.push_back(
            json::object({{"name", "Google DNS"},
                          {"dns1", "8.8.8.8"},
                          {"dns2", "8.8.4.4"}}));

        profiles.push_back(
            json::object({{"name", "ACNH mtu"},
                          {"mtu", 1500}}));

        for (const auto& p : profiles.items()) {
            json values = p.value();
            if (values.find("name") != values.end())
                listItem = new brls::ListItem(values["name"]);
            else
                listItem = new brls::ListItem("Unnamed");
            listItem->getClickEvent()->subscribe([this, values](brls::View* view) {
                brls::Dialog* dialog = new brls::Dialog(values.dump(0).substr(1, values.dump(0).size() - 2));
                brls::GenericEvent::Callback callbackOk = [this, dialog, values](brls::View* view) {
                    nifmInitialize(NifmServiceType_Admin);
                    NifmNetworkProfileData profile;
                    nifmGetCurrentNetworkProfile(&profile);
                    unsigned char buf[sizeof(struct in6_addr)];
                    if (values.find("ip_addr") != values.end()) {
                        if (inet_pton(AF_INET, std::string(values["ip_addr"]).c_str(), buf)) {
                            profile.ip_setting_data.ip_address_setting.is_automatic = u8(0);
                            stringToIp(std::string(values["ip_addr"]), profile.ip_setting_data.ip_address_setting.current_addr.addr);
                        }
                    }
                    if (values.find("subnet_mask") != values.end()) {
                        if (inet_pton(AF_INET, std::string(values["subnet_mask"]).c_str(), buf)) {
                            stringToIp(std::string(values["subnet_mask"]), profile.ip_setting_data.ip_address_setting.subnet_mask.addr);
                        }
                    }
                    if (values.find("gateway") != values.end()) {
                        if (inet_pton(AF_INET, std::string(values["gateway"]).c_str(), buf)) {
                            stringToIp(std::string(values["gateway"]), profile.ip_setting_data.ip_address_setting.gateway.addr);
                        }
                    }
                    if (values.find("dns1") != values.end()) {
                        if (inet_pton(AF_INET, std::string(values["dns1"]).c_str(), buf)) {
                            profile.ip_setting_data.dns_setting.is_automatic = u8(0);
                            stringToIp(std::string(values["dns1"]), profile.ip_setting_data.dns_setting.primary_dns_server.addr);
                        }
                    }
                    if (values.find("dns2") != values.end()) {
                        if (inet_pton(AF_INET, std::string(values["dns2"]).c_str(), buf)) {
                            profile.ip_setting_data.dns_setting.is_automatic = u8(0);
                            stringToIp(std::string(values["dns2"]), profile.ip_setting_data.dns_setting.secondary_dns_server.addr);
                        }
                    }
                    if (values.find("mtu") != values.end()) {
                        profile.ip_setting_data.mtu = u16(values["mtu"]);
                    }
                    if (values.find("ip_auto") != values.end()) {
                        profile.ip_setting_data.ip_address_setting.is_automatic = u8(values["ip_auto"]);
                    }
                    if (values.find("dns_auto") != values.end()) {
                        profile.ip_setting_data.dns_setting.is_automatic = u8(values["dns_auto"]);
                    }

                    nifmSetNetworkProfile(&profile, &profile.uuid);
                    nifmSetWirelessCommunicationEnabled(true);
                    nifmExit();
                    std::this_thread::sleep_for(std::chrono::microseconds(2500000));  //Wait to avoid crashes when leaving too fast
                    dialog->close();
                };
                brls::GenericEvent::Callback callbackNo = [dialog](brls::View* view) {
                    dialog->close();
                };
                dialog->addButton("menus/common/confirm"_i18n, callbackOk);
                dialog->addButton("menus/common/cancel"_i18n, callbackNo);
                dialog->setCancelable(false);
                dialog->open();
            });
            list->addView(listItem);
        }
    }
    this->setContentView(list);
}

std::string NetPage::ipToString(u8* ip)
{
    std::string res = "";
    for (size_t i = 0; i < 3; i++) {
        res += std::to_string(unsigned(ip[i]));
        res += ".";
    }
    res += std::to_string(unsigned(ip[4]));
    return res;
}

int NetPage::stringToIp(const std::string& ip, u8* out)
{
    size_t start;
    size_t end = 0;
    int i = 0;
    while ((start = ip.find_first_not_of(".", end)) != std::string::npos) {
        end = ip.find(".", start);
        out[i] = u8(std::stoi(ip.substr(start, end - start)));
        i++;
    }
    return 0;
}