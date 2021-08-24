# Parrot-Disco-Formation-flight
fly multiple Disco wings with skycontrollers and in formation

<a name="top">![UAVPAL Logo](https://uavpal.com/img/uavpal-logo-cut-461px.png)</a>
# Parrot Disco over 4G/LTE (softmod)

## About
Disco4G is a software modification (softmod) for the Parrot Disco drone. 

My mod uses Disco4g and more specifically the Dailyknowmores's parachute mod fork (V1.2). 



## Community
[![UAVPAL Slack Workspace](https://uavpal.com/img/slack.png)](https://uavpal.com/slack)


## Why?
- simply for fun and to try to fly several Disco(s) controlled by a "master one"

## How does it work?
Two scripts are added to the "parachute mod" in order:
1) spysc2.sh: to capture the Skycontroller2 buttons and sticks values and to send them to the ESP32 board.
2) pud2ESP32.sh: to capture the .pud telemetry of the chuck and to send it to the ESP32 board

3) Then the ESP32 of the master Disco takes care to send these values to all the slave(s) Disco(s).



![alt tag](https://user-images.githubusercontent.com/31324055/130589754-dc2da2e0-f366-434d-aad8-24c96b2bab09.png)

all the communications either use the regular SC2/Chuck Wifi of each Disco AND an ESP-Now communication Master/slave(s) is put in place to tranfer telemtry and sticks values to the slaves.

![alt tag](https://user-images.githubusercontent.com/31324055/130593977-acc94515-f512-46ca-b4f2-8be1f4e9244f.png)

4) Finally, the ESP32 can take the control of the Disco by sending SBUS command to the servo plug.

![alt tag](https://user-images.githubusercontent.com/31324055/130594409-b91f91dd-b71b-4440-a797-20d06bb220e1.png)


## Requirements
*Hardware:*
- [Parrot Disco](https://www.parrot.com/us/drones/parrot-disco) 
- Parrot Skycontroller 2 
- ESP32 boards to put into each disco: https://www.espressif.com/en/products/modules/esp32

*Software:*
- FreeFlight Pro App on tablet/phone (can be the same device providing Wi-Fi tethering)
- LTE disco4g-parachute mod branch 1.2 installed on every Disco: https://github.com/dailyknowmore/disco4g_parachute/tree/v1.2_parachute
- ESP32 software installed on each Disco

## Installation

- Unzip contents (the extraction process automatically creates a new folder "disco4g-parachute_1.2_modf2knpw"
- Ensure Disco and Skycontroller 2 are on the latest Parrot firmware supported by Disco4G, if required you can up-/downgrade the firmware manually (see details for Disco here and Skycontroller 2 here).
- Ensure there is sufficient storage available on the Disco. This softmod only requires 10 MB, however it will not work if 100% of the internal storage is used by videos. This can be checked in FreeFlight Pro.
- Turn on your Disco 
- Ensure no device is plugged into Skycontroller 2 via USB cable.
- Turn on Skycontroller 2 and wait until it's connected to the Disco via Wi-Fi (green LED).
- Connect your PC's WiFi to the Disco (e.g. DISCO-123456).
- Use an FTP tool (e.g. FileZilla) to upload the softmod to your Disco.
  - Connect via FTP to 192.168.42.1 (no username/password required)
  - You have to set the transfer type to Binary (not ASCII). In FileZilla under Edit → Settings... → Transfers → FTP: File Types → Default transfer type → change from Auto to Binary.
  - Transfer the "disco4g-master" or "disco4g-x.y" folder to the "/internal_000" folder on the Disco.
- On Disco firmware 1.7.0 and newer : Double press the Disco's power button to enable the telnet server.
- Open a Command Prompt (Windows)/Terminal (macOS/Linux) and Copy/Paste the following commands manually (line by line) to install the softmod.
  - telnet 192.168.42.1 
  - mount -o remount,rw /
  - mv /data/ftp/internal_000/disco4g-* /tmp/disco4g
  - mv /tmp/disco4g/disco/logsc2.sh logsc2.sh
  - mv /tmp/disco4g/disco/pud2ESP.sh pud2ESP.sh
  - chmod +x ./*.sh
  - chmod +x /tmp/disco4g/*/*_install.sh
  - /tmp/disco4g/disco/disco_install.sh
  - /tmp/disco4g/skycontroller2/skycontroller2_install.sh
- reboot
- Turn off Skycontroller 2 manually

if everything is ok you should see something like this :
![alt tag](https://user-images.githubusercontent.com/31324055/130605174-70636e3d-c44f-495c-85f4-29d3393018de.png)

Now it's time to compile your ESP32 code. Open the ino file into the ESP32 directory
Configure your board:

![alt tag](https://user-images.githubusercontent.com/31324055/130609812-a025bba7-605e-4f81-893b-c6ab7943f85f.png)

- in this example the board is running on Wroom module, is  a Slave, its Disco SSID is DISCO_JP2
- you can change the board from "Wroom32" using pin 13 for SBUS output to "Lolin32 lite" using pin 23
- you can configure DISCO_IS_MASTER as Master or slave or "TEST_NOSLAVE" (simple data logger connected to YOUR_SSID)


## How to fly  (User Manual)
plug each ESP32 into each chuck. If you use a regular servo wire then beware of the polarities...
- Red wire is 5V
- white (or yellow) wire is SBus out pin
- black wire is Gnd
![alt tag](https://user-images.githubusercontent.com/31324055/130612221-cafb50f8-abfd-4096-9ad7-987a0ff1a66d.png)
![alt tag](https://user-images.githubusercontent.com/31324055/130612826-c2084de8-428c-41df-bc2e-31286539a0f7.png)

For easy connection on WROOM32 module you may have to swap the red and black pins on the ESP32 side...

now, start up your discos, start up your skycontrollers. When everything is green, double press on the chucks buttons to enable telnet.

For the moment do not fly with this mod... when pressing right trigger + A on the SkyController, the disco goes into fully manual mode.
DO NOT FLY WITH MANUAL MODE
to exit  press right trigger or enter into loiter (left stick horizontal)

To capture telemetry, the disco must forst take off in "autopilot mode" press the button on the SC2, it will try to take off and quit 10s after...
So you can press the button to take off, immediatlely, go to manual mode (A + right trigger). Then the telemetry (of the master) will be sent to any slave

Currently I did mix master and slaves sticks on the slave to demonstrate that the slave is controlled by its Skycontroller + the master's one !

Work in progress... Don't fly with this now !

## Contact/Contribute


## Special Thanks to
- UAVPAL community :
Join our [UAVPAL Slack workspace](https://uavpal.com/slack) 
(please do not use email for issues/troubleshooting help. Join our Slack community instead!)

- Parachute mod Dailyknowmore : https://github.com/dailyknowmore

## Disclaimer
This is still a VERY EXPERIMENTAL modification! Mod and fly your Disco at YOUR OWN RISK!!!
