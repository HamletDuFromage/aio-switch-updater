#include "net_page.hpp"
#include <iostream>
#include <arpa/inet.h>



namespace i18n = brls::i18n;
using namespace i18n::literals;
using json = nlohmann::json;

NetPage::NetPage() : AppletFrame(true, true)
{
    this->setTitle("menus/net_settings"_i18n );
    
    nifmInitialize(NifmServiceType_User);
    NifmNetworkProfileData profile;
    nifmGetCurrentNetworkProfile (&profile);
    nifmExit();

    int uuid = 0;
    for (int j = 0; j < 16; j++) uuid +=  int(profile.uuid.uuid[j]);

    std::string labelText = "";
    if(uuid){
        if(profile.ip_setting_data.ip_address_setting.is_automatic){
            labelText += "IP Adress: Automatic";
        }
        else{
            labelText = "IP Adress: "       + ipToString(profile.ip_setting_data.ip_address_setting.current_addr.addr)
                        +"\nSubnet Mask: "  + ipToString(profile.ip_setting_data.ip_address_setting.subnet_mask.addr)
                        +"\nGateway: "      + ipToString(profile.ip_setting_data.ip_address_setting.gateway.addr);
        }
        labelText += "\nMTU: " + std::to_string(unsigned(profile.ip_setting_data.mtu));

        if(profile.ip_setting_data.dns_setting.is_automatic){
            labelText += "\nDNS: Automatic";
        }
        else{
            labelText +=    "\nPrimary DNS: "  + ipToString(profile.ip_setting_data.dns_setting.primary_dns_server.addr)
                            +"\nSecondary DNS: "+ ipToString(profile.ip_setting_data.dns_setting.secondary_dns_server.addr);
        }
    }
    else{
        labelText = "Please connect to internet to use this feature.";
    }

    list = new brls::List();
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

    if(uuid){
    std::fstream profilesFile;
    json profiles;
    if(std::filesystem::exists(INTERNET_JSON)){
        profilesFile.open(INTERNET_JSON, std::fstream::in);
        profilesFile >> profiles;
        profilesFile.close();
        profiles.push_back(json::object({
            {"name", "90DNS (Europe)"},
            {"dns1", "163.172.141.219"},
            {"dns2", "207.246.121.77"}
        }));
    }
    else{
        profiles = {{
            {"name", "90DNS (Europe)"},
            {"dns1", "163.172.141.219"},
            {"dns2", "207.246.121.77"}
        }};
    }

    profiles.push_back(json::object({
        {"name", "90DNS (USA)"},
        {"dns1", "207.246.121.77"},
        {"dns2", "163.172.141.219"}
    }));

    profiles.push_back(json::object({
        {"name", "Google DNS"},
        {"dns1", "8.8.8.8"},
        {"dns2", "8.8.4.4"}
    }));

    profiles.push_back(json::object({
        {"name", "Automatic IP Address"},
        {"ip_auto", true}
    }));

    profiles.push_back(json::object({
        {"name", "Automatic DNS"},
        {"dns_auto", true}
    }));

    profiles.push_back(json::object({
        {"ip_addr", "10.13.111.111"},
        {"subnet_mask", "255.255.0.0"},
        {"gateway", "10.13.37.1"},
        {"mtu", 1500},
        {"name", "ACNH lan-play"}
    }));

    int nbProfiles = profiles.size();
    listItems.reserve(nbProfiles);

    int iter = 0;
    for (const auto& p : profiles.items()){
        json values = p.value();
        if(values.find("name") != values.end()) listItems[iter] = new brls::ListItem(values["name"]);
        else listItems[iter] = new brls::ListItem("Unnamed");
        listItems[iter]->getClickEvent()->subscribe([&, values](brls::View* view){
            brls::Dialog* dialog = new brls::Dialog(values.dump(0).substr(1, values.dump(0).size() - 2));
            brls::GenericEvent::Callback callbackOk = [&, dialog, values](brls::View* view) {
                nifmInitialize(NifmServiceType_Admin);
                NifmNetworkProfileData profile;
                nifmGetCurrentNetworkProfile (&profile);
                unsigned char buf[sizeof(struct in6_addr)];
                if(values.find("ip_addr") != values.end()){
                    if(inet_pton(AF_INET, std::string(values["ip_addr"]).c_str(), buf)){
                        profile.ip_setting_data.ip_address_setting.is_automatic = u8(0);
                        //nifmSetNetworkProfile(&profile, &profile.uuid);
                        stringToIp(std::string(values["ip_addr"]), profile.ip_setting_data.ip_address_setting.current_addr.addr);
                    }
                }
                if(values.find("subnet_mask") != values.end()){
                    if(inet_pton(AF_INET, std::string(values["subnet_mask"]).c_str(), buf)){
                        stringToIp(std::string(values["subnet_mask"]), profile.ip_setting_data.ip_address_setting.subnet_mask.addr);
                    }
                }
                if(values.find("gateway") != values.end()){
                    if(inet_pton(AF_INET, std::string(values["gateway"]).c_str(), buf)){
                        stringToIp(std::string(values["gateway"]), profile.ip_setting_data.ip_address_setting.gateway.addr);
                    }
                }
                if(values.find("dns1") != values.end()){
                    if(inet_pton(AF_INET, std::string(values["dns1"]).c_str(), buf)){
                        profile.ip_setting_data.dns_setting.is_automatic = u8(0);
                        stringToIp(std::string(values["dns1"]), profile.ip_setting_data.dns_setting.primary_dns_server.addr);
                        //std::cout << unsigned(profile.ip_setting_data.dns_setting.primary_dns_server.addr[0]) << std::endl;
                    }
                }
                if(values.find("dns2") != values.end()){
                    if(inet_pton(AF_INET, std::string(values["dns2"]).c_str(), buf)){
                        profile.ip_setting_data.dns_setting.is_automatic = u8(0);
                        stringToIp(std::string(values["dns2"]), profile.ip_setting_data.dns_setting.secondary_dns_server.addr);
                    }
                }
                if(values.find("mtu") != values.end()){
                    profile.ip_setting_data.mtu = u16(values["mtu"]);
                    /* try{
                        u8 mtu = u8(std::stoi(std::string(values["mtu"])));
                        profile.ip_setting_data.mtu = mtu;
                    }
                    catch(const std::invalid_argument& ia){} */
                }
                if(values.find("ip_auto") != values.end()){
                    profile.ip_setting_data.ip_address_setting.is_automatic = u8(values["ip_auto"]);
                }
                if(values.find("dns_auto") != values.end()){
                    profile.ip_setting_data.dns_setting.is_automatic = u8(values["dns_auto"]);
                }

                nifmSetNetworkProfile(&profile, &profile.uuid);
                nifmSetWirelessCommunicationEnabled(true);
                nifmExit();
                usleep(2500000); //Wait to avoid crashes when leaving too fast
                dialog->close();
                //brls::Application::pushView(new NetPage());
            };
            brls::GenericEvent::Callback callbackNo = [dialog](brls::View* view) {
                dialog->close();
            };
            dialog->addButton("menus/Confirm_button"_i18n , callbackOk);
            dialog->addButton("menus/Cancel_button"_i18n , callbackNo);
            dialog->setCancelable(false);
            dialog->open();
        });
        list->addView(listItems[iter]);
        iter++;
    }
    }
    else{
        cancel = new brls::ListItem("menus/go_back"_i18n);
        cancel->getClickEvent()->subscribe([&](brls::View* view){
            brls::Application::pushView(new MainFrame());
        });
        list->addView(cancel);
    }
    this->setContentView(list);
}

std::string NetPage::ipToString(u8* ip){
    std::string res = "";
    for (size_t i = 0; i < 3; i++){
        res += std::to_string(unsigned(ip[i]));
        res += ".";
    }
    res += std::to_string(unsigned(ip[4]));
    return res;
}

int NetPage::stringToIp(std::string ip, u8 *out){
    size_t start;
    size_t end = 0;
    int i = 0;
    while ((start = ip.find_first_not_of(".", end)) != std::string::npos)
    {
        end = ip.find(".", start);
        out[i] = u8(std::stoi(ip.substr(start, end - start)));
        i++;
    }
    return 0;
}