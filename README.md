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

Then the ESP32 of the master Disco insures to send these values to all the slave(s) Disco(s).



![alt tag](https://user-images.githubusercontent.com/31324055/130589754-dc2da2e0-f366-434d-aad8-24c96b2bab09.png)

all the communications either use the regular SC2/Chuck Wifi of each Disco AND an ESP-Now communication Master/slave(s) is put in place to tranfer telemtry and sticks values to the slaves.

![alt tag](https://user-images.githubusercontent.com/31324055/130592459-87fde9ee-8b97-44d8-9ed9-da6b29600971.png)


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
to install the mod Please see Wiki article [Installation](https://github.com/uavpal/disco4g/wiki/Installation).

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
