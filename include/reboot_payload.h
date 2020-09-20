#ifndef __REBOOT_PAYLOAD_H__
#define __REBOOT_PAYLOAD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <switch.h>

int reboot_to_payload(const char* path);

#ifdef __cplusplus
}
#endif

#endif