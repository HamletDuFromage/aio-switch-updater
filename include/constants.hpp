#pragma once

#define ROOT_PATH           "/"
#define APP_PATH            "/switch/aio-switch-updater/"
#define NRO_PATH            "/switch/aio-switch-updater/aio-switch-updater.nro"
#define DOWNLOAD_PATH       "/config/aio-switch-updater/"
#define CONFIG_PATH         "/config/aio-switch-updater/"
#define CONFIG_PATH_UNZIP   "config\\aio-switch-updater"

#define RCM_PAYLOAD_PATH    "romfs:/aio_rcm.bin"

#define APP_URL             "https://github.com/HamletDuFromage/aio-switch-updater/releases/latest/download/aio-switch-updater.zip"
#define TAGS_INFO           "https://api.github.com/repos/HamletDuFromage/aio-switch-updater/tags"
#define APP_FILENAME         "/config/aio-switch-updater/app.zip"

#define SIGPATCHES_URL      "https://hamletdufromage.github.io/sigpatches-updater/sigpatches.html"
#define SIGPATCHES_FILENAME "/config/aio-switch-updater/sigpatches.zip"
#define HEKATE_IPL_PATH     "/bootloader/hekate_ipl.ini"

#define FIRMWARE_URL        "https://hamletdufromage.github.io/switch-fw/firmwares.html"
#define FIRMWARE_FILENAME   "/config/aio-switch-updater/firmware.zip"
#define FIRMWARE_PATH       "/firmware/"

#define CFW_URL             "https://hamletdufromage.github.io/switch-cfw/cfw.html"
#define CFW_FILENAME        "/config/aio-switch-updater/cfw.zip"

#define AMS_URL             "https://hamletdufromage.github.io/switch-cfw/ams.html"
#define AMS_FILENAME        "/config/aio-switch-updater/ams.zip"

#define HEKATE_URL          "https://hamletdufromage.github.io/switch-cfw/hekate.html"

#define PAYLOAD_URL         "https://hamletdufromage.github.io/switch-payloads/payloads.html"

#define CHEATS_RELEASE_URL  "https://github.com/HamletDuFromage/switch-cheats-db/releases/tag/v1.0"
#define CHEATS_URL_TITLES   "https://github.com/HamletDuFromage/switch-cheats-db/releases/download/v1.0/titles.zip"
#define CHEATS_URL_CONTENTS "https://github.com/HamletDuFromage/switch-cheats-db/releases/download/v1.0/contents.zip"
#define CHEATS_FILENAME     "/config/aio-switch-updater/cheats.zip"
#define CHEATS_EXCLUDE      "/config/aio-switch-updater/exclude.txt"
#define UPDATED_TITLES_PATH "/config/aio-switch-updater/updated.dat"
#define CHEATS_VERSION      "/config/aio-switch-updater/cheats_version.dat"
#define AMS_CONTENTS        "/atmosphere/contents/"
#define REINX_CONTENTS      "/ReiNX/contents/"
#define SXOS_TITLES         "/sxos/titles/"
#define AMS_PATH            "/atmosphere/"
#define SXOS_PATH           "/sxos/"
#define REINX_PATH          "/ReiNX/"
#define CONTENTS_PATH       "contents/"
#define TITLES_PATH         "titles/"

#define COLOR_PROFILES_PATH "/config/aio-switch-updater/jc_profiles.json"

#define PAYLOAD_PATH        "/payloads/"
#define BOOTLOADER_PATH     "/bootloader"
#define BOOTLOADER_PL_PATH  "/bootloader/payloads/"
#define UPDATE_BIN_PATH     "/bootloader/update.bin"
#define REBOOT_PAYLOAD_PATH "/atmosphere/reboot_payload.bin"

#define LISTITEM_HEIGHT     50


enum archiveType{
    sigpatches,
    cheats,
    fw,
    app,
    cfw,
    ams_cfw,
};

enum CFW{
    rnx,
    sxos,
    ams,
};