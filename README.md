# AIO-Switch-Updater
<!-- ![hbappstore](https://img.shields.io/endpoint?url=https%3A%2F%2Frunkit.io%2Fhomlet%2Fhbappstore-shieldsio%2Fbranches%2Fmaster%3Furl%3Dhttps%3A%2F%2Fswitchbru.com%2Fappstore%2Frepo.json%26name%3Daioswitchupdater) -->
![releases](https://img.shields.io/github/downloads/HamletDuFromage/AIO-switch-updater/total)
![GitHub Workflow Status](https://img.shields.io/github/workflow/status/HamletDuFromage/AIO-switch-updater/Build%20AIO-switch-updater)
![tag](https://img.shields.io/github/v/release/HamletDuFromage/AIO-switch-updater)
![GitHub](https://img.shields.io/github/license/HamletDuFromage/aio-switch-updater)

<a href="https://liberapay.com/HamletDuFromage/donate"><img alt="Donate using Liberapay" src="https://liberapay.com/assets/widgets/donate.svg"></a>
[![btc](https://img.shields.io/badge/BTC-1CoFc1bY5AHLP6Noe1zmqnJnp7ZWBxyo79-yellow)](https://github.com/HamletDuFromage/aio-switch-updater#like-the-app)
[![eth](https://img.shields.io/badge/ETH-0xf68f568e21a15934e0e9a6949288c3ca009140ba-purple)](https://github.com/HamletDuFromage/aio-switch-updater#like-the-app)

<!-- ([![ko-fi](https://img.shields.io/badge/ko--fi-buy%20me%20a%20coffee-ff69b4)](https://ko-fi.com/hamletdufromage)) -->

<p align="center">
<img src = "https://user-images.githubusercontent.com/61667930/93691188-7833f000-fad1-11ea-866d-42e19be54425.jpg"\><br>
</p>

A Nintendo Switch homebrew app to download and update CFWs, sigpatches, FWs and cheat codes. Supports Atmosphère, ReiNX and SXOS.

Works on **unpatched** (Erista) and **patched** (v2/Mariko) Switches.

## How to install
Copy the `aio-switch-updater/` directory to `/switch/` on your sdcard.

## Features
### ⬦ Update CFW
- Update the Atmosphère Switch Custom Firmware. AIO-Switch-Updater uses a custom RCM payload to finalise the install as it can't be performed while HOS is running.
  - If you would like to preserve additional files or directories, write their path (one line each) in `/config/aio-switch-updater/preserve.txt` and they won't be overwritten when updating.
  - Place [this file](https://github.com/HamletDuFromage/aio-switch-updater/blob/master/copy_files.txt) in `/config/aio-switch-updater/copy_files.txt` in order to have specific copy operations performed after each download. This is mainly meant for users with trinkets who want payloads automatically copied to a directory.
  - A custom Atmosphère url can be entered in [this file](https://github.com/HamletDuFromage/aio-switch-updater/blob/master/customPacks.json). Once moved to `/config/aio-switch-updater/customPacks.json`, it will show on the `Update Atmopshère` menu. This can be used to support third-party packs through AIO-Switch-Updater

### ⬦ Update Hekate/Payload
- Download and update Hekate, as well as a selection of RCM payloads

### ⬦ Update Sigpatches
- Dowload sigpatches, which are patches required to launch unofficial .NSPs on the Atmosphère CFW.

### ⬦ Download firmwares
- Download firmware files to `/firmware` that can then be installed using DayBreak.

### ⬦ Download cheats
- Download and extract daily-updated cheat code. The program will only extract cheat codes for the games you own. By default, this homebrew will overwrite the existing cheats. If you have your own cheat files that you'd like to keep as is, you can turn off cheat updates for specific titles in `Tools->Cheat Menu`.
- Download cheat sheets from [Cheat Slips](https://www.cheatslips.com/). 
- Download individual cheat codes from the `GBAtemp.net` database.

## Extras (in the `Tools` tab)
- Reboot to specific payload.
- Change software color scheme of Joy-Cons. Additional color profiles can be found in the releases and should be copied to `config/aio-switch-updater/jc_profiles.json`. Use [this webpage](https://hamletdufromage.github.io/JC-color-picker/JCpicker.html) to generate your own profiles.
- Change software color scheme of Pro Controllers (has to be paired as Player 1). Additional color profiles can be found in the releases and should be copied to `config/aio-switch-updater/pc_profiles.json`.
- Consult installed cheat codes.
- Launch the Switch's web browser.
- Edit internet settings (DNS, IP address, MTU, etc). Add you own configs to `config/aio-switch-updater/internet.json`. You can find a template in the root of the repo.
- Tabs can be hidden through the `Hide tabs` menu, and more entries can be hidden by manually editing [`config/aio-switch-updater/hide_tabs.json`](https://github.com/HamletDuFromage/aio-switch-updater/blob/master/hide_tabs.json).

## Screenshots
<details><summary>Expand to view the screenshots</summary>

![update_ams](https://user-images.githubusercontent.com/61667930/125843308-94666f64-c538-465f-88b1-10dbc1e2024e.jpg)
![deepsea_builder](https://user-images.githubusercontent.com/61667930/125843328-829d9e92-617a-4a92-b85f-2f2abd096f5f.jpg)
![downloading](https://user-images.githubusercontent.com/61667930/125843333-e18faf45-6034-4dfa-841b-3e20259e4dc9.jpg)
![get_cheats](https://user-images.githubusercontent.com/61667930/125843305-4f1b4a91-f182-4e15-9d5b-af0e2ec7fd9e.jpg)
![get_individual_cheat](https://user-images.githubusercontent.com/61667930/125843316-0527d3f2-5b6e-42da-83ef-0f8e67646eb3.jpg)
![tools_menu](https://user-images.githubusercontent.com/61667930/125843312-394cc38e-de75-4f71-942d-9d38d3d37b29.jpg)
![joycon_color_swapper](https://user-images.githubusercontent.com/61667930/125843319-c81d372e-58dc-432b-8f8c-828f768ae817.jpg)

</details>

## Build

<details><summary>Expand to view the build instructions</summary>

You need to have installed devkitPro and devkitARM in order to compile this project.

Install the required dependencies:
```bash
$ sudo (dkp-)pacman -Sy
```
```bash
$ sudo (dkp-)pacman -S  switch-glfw \
                        switch-curl \
                        switch-glm \
                        switch-mbedtls \
                        switch-zlib
```
Use [`switch-ex-curl`](https://github.com/eXhumer/switch-ex-curl) instead of `switch-curl` to use this app with an invalid SSL certificate.

Clone the repository
```bash
$ git clone --recursive https://github.com/HamletDuFromage/aio-switch-updater
$ cd aio-switch-updater
```

Compile 
```bash
$ cd aiosu-forwarder
$ make
$ cd ..
$ make
```

</details>

## Contribute

PRs and suggestions are encouraged! If you wish to help with the localization of the app, you can translate the files in `resources/i18n/`. To easily find the non-translated strings and translate them, you may use `localizer.py` (e.g. `python localizer.py -r resources//i18n//en-US//menus.json -w resources//i18n//fr//menus.json`).

## Disclaimer
I do not own, host nor distribute any of the files that can be downloaded with this homebrew tool. At the owner's request, I will immediately remove the ability to download any problematic file.

## Special thanks
- [natinusala](https://github.com/natinusala) for the Borealis library.
- [tiansongyu](https://github.com/tiansongyu) for bringing support for multi-language and for his Chinese translation.
- [yyoossk](https://github.com/yyoossk) for the Japanese locale.
- [sergiou87](https://github.com/sergiou87) for the Spanish locale.
- [pedruhb](https://github.com/pedruhb) for the Brazilian locale.
- [AD2076](https://github.com/AD2076) for the Italian locale.
- [qazrfv1234](https://github.com/qazrfv1234) for the Traditional Chinese locale.
- [Nota Inutilis](https://github.com/NotaInutilis/) for the French locale.
- [Team Neptune](https://github.com/Team-Neptune) whose rcm code I used.
- [fennectech](https://github.com/fennectech) for helping test the app and providing suggestions.
- Iliak for [Cheat Slips](https://www.cheatslips.com/).

### Like the app?
- Liberapay : <a href="https://liberapay.com/HamletDuFromage/donate"><img alt="Donate using Liberapay" src="https://liberapay.com/assets/widgets/donate.svg"></a>
- BTC: 1CoFc1bY5AHLP6Noe1zmqnJnp7ZWBxyo79
- ETH: 0xf68f568e21a15934e0e9a6949288c3ca009140ba
