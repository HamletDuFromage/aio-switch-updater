# AIO-switch-updater
![releases](https://img.shields.io/github/downloads/HamletDuFromage/AIO-switch-updater/total)
![GitHub Workflow Status](https://img.shields.io/github/workflow/status/HamletDuFromage/AIO-switch-updater/Build%20AIO-switch-updater)
![tag](https://img.shields.io/github/v/release/HamletDuFromage/AIO-switch-updater)
![GitHub](https://img.shields.io/github/license/HamletDuFromage/aio-switch-updater)

[![btc](https://img.shields.io/badge/BTC-1CoFc1bY5AHLP6Noe1zmqnJnp7ZWBxyo79-yellow)](https://github.com/HamletDuFromage/aio-switch-updater#like-the-app)
[![eth](https://img.shields.io/badge/ETH-0xf68f568e21a15934e0e9a6949288c3ca009140ba-purple)](https://github.com/HamletDuFromage/aio-switch-updater#like-the-app)

[//]: ([![ko-fi](https://img.shields.io/badge/ko--fi-buy%20me%20a%20coffee-ff69b4)](https://ko-fi.com/hamletdufromage))


All-in-One Nintendo Switch Updater
<p align="center">
<img src = "https://user-images.githubusercontent.com/61667930/93691188-7833f000-fad1-11ea-866d-42e19be54425.jpg"\><br>
</p>

A Nintendo Switch homebrew app to download and update CFWs, sigpatches, FWs and cheat codes. Supports Atmosphère, ReiNX and SXOS.

## How to install
Copy the `aio-switch-updater/` directory to `/switch/` on your sdcard.

## Description of the features
### ⬦ Update CFW
- Download the most popular Switch CFWs. After downloading the CFW archive, the program will ask you whether you want to override your existing .ini files. If you would like to preserve additional files or directories, write their path (one line each) in `/config/aio-switch-updater/preserve.txt` and they won't be overwritten when updating.

### ⬦ Update Sigpatches
- For Atmosphère. Downloads sigpatches, which are patches required for launching unofficial .NSPs. Both AMS and Hekate+AMS sigpatches are available. After downloading the sigpatches archive, the program will ask you whether you want to override your existing .ini files.

### ⬦ Download firmwares
- Download firmare files to `/firmware` that can then be installed using DayBreak or ChoiDuJour.

### ⬦ Download cheats
- Downloads and extracts daily-updated cheat code. The program will only extract cheat codes for the games you own. By default, this homebrew will overwrite the existing cheats. If you have your own cheat files that you'd like to keep as is, you can turn off cheat updates for specific titles in `Tools->Cheat Menu`.
- Since v2.0.0, aio-switch-updater can also download cheat sheets from [`CheatSlips.com`](https://www.cheatslips.com/). 

## Extras (in the `Tools` tab)
- Reboot to specific payload.
- Download payloads to `/bootloader/payloads`.
- Change software color scheme of Joy-Cons. Additional color profiles can be found in the releases and should be copied to `config/aio-switch-updater/jc_profiles.json`. Use [this webpage](https://hamletdufromage.github.io/JC-color-picker/JCpicker.html) to generate your own profiles.
- Change software color scheme of Pro Controllers (has to be paired as Player 1). Additional color profiles can be found in the releases and should be copied to `config/aio-switch-updater/pc_profiles.json`.
- View which of your games got cheat codes from the app.
- Launch the Switch's web browser.
- Edit internet settings (DNS, IP address, MTU, etc). Add you own configs to `config/aio-switch-updater/internet.json`. You can find a template in the root of the repo.

## Screenshots
![ss](https://user-images.githubusercontent.com/61667930/107124480-7a41f400-68a4-11eb-9a01-d7b3c9f3e828.jpg)
![ss](https://user-images.githubusercontent.com/61667930/93721670-42e6db00-fb81-11ea-9f94-1308898398f0.jpg)
![ss](https://user-images.githubusercontent.com/61667930/93721673-437f7180-fb81-11ea-9256-377575148a40.jpg)
![ss](https://user-images.githubusercontent.com/61667930/93691404-3193c500-fad4-11ea-9647-927c979960bc.jpg)
![ss](https://user-images.githubusercontent.com/61667930/105404737-98fc8580-5c2a-11eb-9efb-eb6e69d82b7b.jpg)
![ss](https://user-images.githubusercontent.com/61667930/107124438-364eef00-68a4-11eb-97d4-57df6d8aaa9d.jpg)
![ss](https://user-images.githubusercontent.com/61667930/105404840-b9c4db00-5c2a-11eb-8385-48454465063c.jpg)
![ss](https://user-images.githubusercontent.com/61667930/105407043-80da3580-5c2d-11eb-8f35-27f77079ea53.jpg)
![ss](https://user-images.githubusercontent.com/61667930/105403520-250dad80-5c29-11eb-95e6-d9ab3822d1d6.jpg)

## Build
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
- [Team Neptune](https://github.com/Team-Neptune) whose rcm code I used.
- [fennectech](https://github.com/fennectech) for helping test the app and providing suggestions.
- Iliak for cheatslips.com.

### Like the app?
- BTC: 1CoFc1bY5AHLP6Noe1zmqnJnp7ZWBxyo79
- ETH: 0xf68f568e21a15934e0e9a6949288c3ca009140ba
- CHAINLINK: 0xf68f568e21a15934e0e9a6949288c3ca009140ba