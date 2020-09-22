# AIO-switch-updater
![releases](https://img.shields.io/github/downloads/HamletDuFromage/AIO-switch-updater/total)
![tag](https://img.shields.io/github/v/release/HamletDuFromage/AIO-switch-updater)
![GitHub](https://img.shields.io/github/license/HamletDuFromage/aio-switch-updater)
[![ko-fi](https://img.shields.io/badge/paypal-donate-blue)](https://www.paypal.com/donate?token=El4U5IrRyplctLb9D0jcbitsx-B0oOoWcyetXtaG0hHfIDsruvhUMKqKKR4fz_TzYwvj3tugFhrux_3j)


All-in-One Nintendo Switch Updater
<p align="center">
<img src = "https://user-images.githubusercontent.com/61667930/93691188-7833f000-fad1-11ea-866d-42e19be54425.jpg"\><br>
</p>

A Nintendo Switch homebrew app download and update CFWs, sigpatches, FWs and cheat codes. Support Atmosphere, ReiNX and SXOS.

## How to install
Copy the `aio-switch-updater/` directory to `/switch/` in your sdcard

## Description of the features
### ⬦ Update CFW
Download the most popular Switch CFWs. After downloading the CFW archive, the program will ask you whether you want to override your existing .ini files.

### ⬦ Update Sigpatches
For Atmosphere. Downloads sigpatches, which are patches required for launching unofficial .NSPs. Both AMS and Hekate+AMS sigpatches are available. After downloading the sigpatches archive, the program will ask you whether you want to override your existing .ini files.

### ⬦ Download firmares
Download firmare files to `/firmware` that can then be installed using DayBreak or ChoiDuJour.

### ⬦ Download cheats
Downloads and extracts daily-updated cheat code. The program will only extract cheat codes for the games you own. By default, this homebrew will overwrite the existing cheats. If you have your own cheat files that you'd like to keep as is, you may add the title IDs of the corresponding games, one ID per line, to `/config/cheats-updater/exclude.txt`. Use http://tinfoil.io/title to find the title ID of a game.

## Extras (in the `Tools` tab)
- Reboot to specific payload.
- Download payloads to `/bootloader/payloads`.
- Change software color scheme of Joy-Cons. Additional color profiles can be found in the releases and should be copied to `config/aio-switch-updater/jc_profiles.json`.
- View which of your games got cheat codes from the app.

## Screenshots
![ss](https://user-images.githubusercontent.com/61667930/93691403-30fb2e80-fad4-11ea-9701-7992a1de53e0.jpg)
![ss](https://user-images.githubusercontent.com/61667930/93691404-3193c500-fad4-11ea-9647-927c979960bc.jpg)
![ss](https://user-images.githubusercontent.com/61667930/93721670-42e6db00-fb81-11ea-9f94-1308898398f0.jpg)
![ss](https://user-images.githubusercontent.com/61667930/93721673-437f7180-fb81-11ea-9256-377575148a40.jpg)
![ss](https://user-images.githubusercontent.com/61667930/93691407-322c5b80-fad4-11ea-8879-78047724d9e7.jpg)
![ss](https://user-images.githubusercontent.com/61667930/93691465-16758500-fad5-11ea-8a5c-c0f9694cfb0e.jpg)


## Disclaimer
I do not own, host nor distribute any of the files that can be downloaded with this homebrew tool. At the owner's request, I will immediately remove the ability to download the problematic files.

## Like the app?

<form action="https://www.paypal.com/cgi-bin/webscr" method="post" target="_top">
<input type="hidden" name="cmd" value="_s-xclick" />
<input type="hidden" name="hosted_button_id" value="L6LJAV6FVHNKA" />
<input type="image" src="https://www.paypalobjects.com/en_US/FR/i/btn/btn_donateCC_LG.gif" border="0" name="submit" title="PayPal - The safer, easier way to pay online!" alt="Donate with PayPal button" />
<img alt="" border="0" src="https://www.paypal.com/en_FR/i/scr/pixel.gif" width="1" height="1" />
</form>


