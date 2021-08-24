# Parrot-Disco-Formation-flight
fly multiple Disco wings with skycontrollers and in formation

<a name="top">![UAVPAL Logo](https://uavpal.com/img/uavpal-logo-cut-461px.png)</a>
# Parrot Disco over 4G/LTE (softmod)

## About
Disco4G is a software modification (softmod) for the Parrot Disco drone. 

My mod uses Disco4g (which must be installed) and more specifically the Dailyknowmores's parachute mod fork.



## Community
[![UAVPAL Slack Workspace](https://uavpal.com/img/slack.png)](https://uavpal.com/slack)


## Why?
- simply for fun and to try to fly several Disco controller by a "master one"

## How does it work?
Two scripts are added to this "parachute mod" in order to:
1) capture the Skycontroller2 buttons and sticks values and to send them to the ESP32 board.
2) capture the .pud telemetry of the chuck and to send it to the ESP32 board

3) Then the ESP32 of the master Disco insures to send these values to all the slave(s) Disco(s).



![alt tag](https://user-images.githubusercontent.com/31324055/130589754-dc2da2e0-f366-434d-aad8-24c96b2bab09.png)

all the communications either use the regular SC2/Chuck Wifi of each Disco AND an ESP-Now communication Master/slave(s) is put in place to tranfer telemtry and sticks values to the slaves.

![alt tag](https://user-images.githubusercontent.com/31324055/130593977-acc94515-f512-46ca-b4f2-8be1f4e9244f.png)

4) Finally, the ESP32 can take the control of the Disco by sending SBUS command to the servo plug.

![alt tag](https://user-images.githubusercontent.com/31324055/130594409-b91f91dd-b71b-4440-a797-20d06bb220e1.png)


## Requirements
*Hardware:*
- [Parrot Disco](https://www.parrot.com/us/drones/parrot-disco) 
- Parrot Skycontroller 2 
- ESP32 boards to put into each disco

*Software:*
- FreeFlight Pro App on tablet/phone (can be the same device providing Wi-Fi tethering)
- LTE disco4g-parachute mod branch 1.2 installed on every Disco: https://github.com/dailyknowmore/disco4g_parachute/tree/v1.2_parachute
- ESP32 software installed on each Disco

## Installation
to install the soft mod Please see Wiki article [Installation](https://github.com/uavpal/disco4g/wiki/Installation).

•	Unzip contents (the extraction process automatically creates a new folder "disco4g-master" or "disco4g-x.y", where by x.y is the release version number)
•	Ensure Disco and Skycontroller 2 are on the latest Parrot firmware supported by Disco4G, if required you can up-/downgrade the firmware manually (see details for Disco here and Skycontroller 2 here).
•	Ensure there is sufficient storage available on the Disco. This softmod only requires 10 MB, however it will not work if 100% of the internal storage is used by videos. This can be checked in FreeFlight Pro.
•	Turn on your Disco 
•	Ensure no device is plugged into Skycontroller 2 via USB cable.
•	Turn on Skycontroller 2 and wait until it's connected to the Disco via Wi-Fi (green LED).
•	Connect your PC's WiFi to the Disco (e.g. DISCO-123456).
•	Use an FTP tool (e.g. FileZilla) to upload the softmod to your Disco.
o	Connect via FTP to 192.168.42.1 (no username/password required)
o	You have to set the transfer type to Binary (not ASCII). In FileZilla under Edit → Settings... → Transfers → FTP: File Types → Default transfer type → change from Auto to Binary.
o	Transfer the "disco4g-master" or "disco4g-x.y" folder to the "/internal_000" folder on the Disco.
•	On Disco firmware 1.7.0 and newer : Double press the Disco's power button to enable the telnet server.
•	Open a Command Prompt (Windows)/Terminal (macOS/Linux) and Copy/Paste the following commands manually (line by line) to install the softmod.
telnet 192.168.42.1 (If you are getting "command not found" or "... is not recognized as an internal or external command", please check this FAQ article first)
mv /data/ftp/internal_000/disco4g-* /tmp/disco4g
chmod +x /tmp/disco4g/*/*_install.sh
/tmp/disco4g/disco/disco_install.sh
/tmp/disco4g/skycontroller2/skycontroller2_install.sh
o	reboot
•	Turn off Skycontroller 2 manually


## How to fly  (User Manual)
to be writen

## Contact/Contribute


## Special Thanks to
- UAVPAL community :
Join our [UAVPAL Slack workspace](https://uavpal.com/slack) 
(please do not use email for issues/troubleshooting help. Join our Slack community instead!)

- Parachute mod Dailyknowmore : https://github.com/dailyknowmore

## Disclaimer
This is still a VERY EXPERIMENTAL modification! Mod and fly your Disco at YOUR OWN RISK!!!
