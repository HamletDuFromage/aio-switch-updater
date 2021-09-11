#ifndef __REBOOT_PAYLOAD_H__
#define __REBOOT_PAYLOAD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <switch.h>

int reboot_to_payload(const char* path, bool legacy);

#ifdef __cplusplus
}
#endif

#endif