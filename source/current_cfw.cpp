#include "current_cfw.hpp"
#include <switch.h>

bool isServiceRunning(const char *serviceName) {
    Handle handle;
    SmServiceName service_name = smEncodeName(serviceName);
    bool running = R_FAILED(smRegisterService(&handle, service_name, false, 1));
    svcCloseHandle(handle);
    if (!running)
        smUnregisterService(service_name);

    return running;
}

Result smAtmosphereHasService(bool *out, SmServiceName name) {
    u8 tmp = 0;
    Result rc = serviceDispatchInOut(smGetServiceSession(), 65100, name, tmp);
    if (R_SUCCEEDED(rc) && out)
        *out = tmp;
    return rc;
}

CFW getCFW(){
    bool res = false;
    if(R_SUCCEEDED(smAtmosphereHasService(&res, smEncodeName("rnx")))) {
        if(res) 
            return rnx;
        smAtmosphereHasService(&res, smEncodeName("tx"));
        if(res)
            return sxos;
    }
    else { // use old method just in case
        if(isServiceRunning("rnx"))     return rnx;
        if(isServiceRunning("tx"))      return sxos;
    }
    return ams;
}