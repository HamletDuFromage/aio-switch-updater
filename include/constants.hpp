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

#define SIGPATCHES_URL      "https://raw.githubusercontent.com/HamletDuFromage/nx-links/master/sigpatches.json"
#define SIGPATCHES_FILENAME "/config/aio-switch-updater/sigpatches.zip"
#define HEKATE_IPL_PATH     "/bootloader/hekate_ipl.ini"

#define FIRMWARE_URL        "https://raw.githubusercontent.com/HamletDuFromage/nx-links/master/firmwares.json"
#define FIRMWARE_FILENAME   "/config/aio-switch-updater/firmware.zip"
#define FIRMWARE_PATH       "/firmware/"

#define CFW_URL             "https://raw.githubusercontent.com/HamletDuFromage/nx-links/master/cfw.json"
#define CFW_FILENAME        "/config/aio-switch-updater/cfw.zip"

#define AMS_URL             "https://raw.githubusercontent.com/HamletDuFromage/nx-links/master/ams.json"
#define SXOS_URL            "https://raw.githubusercontent.com/HamletDuFromage/nx-links/master/sxos.json"
#define AMS_FILENAME        "/config/aio-switch-updater/ams.zip"

#define HEKATE_URL          "https://raw.githubusercontent.com/HamletDuFromage/nx-links/master/hekate.json"

#define PAYLOAD_URL         "https://raw.githubusercontent.com/HamletDuFromage/nx-links/master/payloads.json"

#define CHEATS_RELEASE_URL  "https://github.com/HamletDuFromage/switch-cheats-db/releases/tag/v1.0"
#define CHEATS_URL_TITLES   "https://github.com/HamletDuFromage/switch-cheats-db/releases/download/v1.0/titles.zip"
#define CHEATS_URL_CONTENTS "https://github.com/HamletDuFromage/switch-cheats-db/releases/download/v1.0/contents.zip"
#define LOOKUP_TABLE_URL    "https://raw.githubusercontent.com/HamletDuFromage/switch-cheats-db/master/versions.json"
#define CHEATSLIPS_CHEATS_URL "https://www.cheatslips.com/api/v1/cheats/"
#define CHEATSLIPS_TOKEN_URL "https://www.cheatslips.com/api/v1/token"
#define TOKEN_PATH          "/config/aio-switch-updater/token.json"
#define CHEATS_FILENAME     "/config/aio-switch-updater/cheats.zip"
#define CHEATS_EXCLUDE      "/config/aio-switch-updater/exclude.txt"
#define FILES_IGNORE        "/config/aio-switch-updater/preserve.txt"
#define INTERNET_JSON       "/config/aio-switch-updater/internet.json"
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
#define PC_COLOR_PATH       "/config/aio-switch-updater/pc_profiles.json"

#define PAYLOAD_PATH        "/payloads/"
#define BOOTLOADER_PATH     "/bootloader"
#define BOOTLOADER_PL_PATH  "/bootloader/payloads/"
#define UPDATE_BIN_PATH     "/bootloader/update.bin"
#define REBOOT_PAYLOAD_PATH "/atmosphere/reboot_payload.bin"
#define FUSEE_SECONDARY     "/atmosphere/fusee-secondary.bin"
#define FUSEE_MTC           "/atmosphere/fusee-mtc.bin"

#define AMS_ZIP_PATH        "/config/aio-switch-updater/ams.zip"
#define APP_ZIP_PATH        "/config/aio-switch-updater/app.zip"
#define CFW_ZIP_PATH        "/config/aio-switch-updater/cfw.zip"
#define FW_ZIP_PATH         "/config/aio-switch-updater/firmware.zip"
#define CHEATS_ZIP_PATH     "/config/aio-switch-updater/cheats.zip"
#define SIGPATCHES_ZIP_PATH "/config/aio-switch-updater/sigpatches.zip"
#define AMS_DIRECTORY_PATH  "/config/aio-switch-updater/atmosphere/"
#define SEPT_DIRECTORY_PATH "/config/aio-switch-updater/sept/"
#define FW_DIRECTORY_PATH   "/firmware/"

#define HIDE_TABS_JSON      "/config/aio-switch-updater/hide_tabs.json"
#define COPY_FILES_JSON     "/config/aio-switch-updater/copy_files.json"

#define ROMFS_FORWARDER     "romfs:/aiosu-forwarder.nro"
#define FORWARDER_PATH      "/config/aio-switch-updater/aiosu-forwarder.nro"

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