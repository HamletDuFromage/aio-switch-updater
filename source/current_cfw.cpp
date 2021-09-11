#include "current_cfw.hpp"

#include <switch.h>

namespace CurrentCfw {

    namespace {

        bool isServiceRunning(const char* serviceName)
        {
            Handle handle;
            SmServiceName service_name = smEncodeName(serviceName);
            bool running = R_FAILED(smRegisterService(&handle, service_name, false, 1));
            svcCloseHandle(handle);
            if (!running)
                smUnregisterService(service_name);

            return running;
        }

        Result smAtmosphereHasService(bool* out, SmServiceName name, bool v019)
        {
            u8 tmp = 0;
            Result rc = v019 ? tipcDispatchInOut(smGetServiceSessionTipc(), 65100, name, tmp) : serviceDispatchInOut(smGetServiceSession(), 65100, name, tmp);
            if (R_SUCCEEDED(rc) && out)
                *out = tmp;
            return rc;
        }

        bool isPost019()
        {
            u64 version;
            if (R_SUCCEEDED(splGetConfig((SplConfigItem)65000, &version))) {
                if (((version >> 56) & ((1 << 8) - 1)) > 0 || ((version >> 48) & ((1 << 8) - 1)) >= 19) {
                    return true;
                }
            }
            return false;
        }
    }  // namespace

    CFW getCFW()
    {
        bool res = false;
        bool v019 = isPost019();  //AMS v0.19 introduced sm changes, and as such old AMS versions have to be treated differently
        if (R_SUCCEEDED(smAtmosphereHasService(&res, smEncodeName("rnx"), v019))) {
            if (res)
                return CFW::rnx;
            smAtmosphereHasService(&res, smEncodeName("tx"), v019);
            if (res)
                return CFW::sxos;
        }
        else {  // use old method just in case
            if (isServiceRunning("rnx")) return CFW::rnx;
            if (isServiceRunning("tx")) return CFW::sxos;
        }
        return CFW::ams;
    }

    std::string getAmsInfo()
    {
        u64 version;
        std::string res;
        if (R_SUCCEEDED(splGetConfig((SplConfigItem)65000, &version))) {
            res += std::to_string((version >> 56) & ((1 << 8) - 1)) + "." +
                   std::to_string((version >> 48) & ((1 << 8) - 1)) + "." +
                   std::to_string((version >> 40) & ((1 << 8) - 1));
            if (R_SUCCEEDED(splGetConfig((SplConfigItem)65007, &version)))
                res += version ? "|E" : "|S";
            return res;
        }
        else
            return "Couldn't retrieve AMS version";
    }

}  // namespace CurrentCfw