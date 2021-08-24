# Parrot-Disco-Formation-flight
fly multiple Disco wings with skycontrollers and in formation

<a name="top">![UAVPAL Logo](https://uavpal.com/img/uavpal-logo-cut-461px.png)</a>
# Parrot Disco over 4G/LTE (softmod)

## About
Disco4G is a software modification (softmod) for the Parrot Disco drone. Instead of the built-in regular Wi-Fi, it allows to use a 4G/LTE cellular/mobile network connection to link Skycontroller 2 to the Disco. Control/telemetry and live video stream are routed through the 4G/LTE connection. In other words, range limit becomes your imagination! Ok, to be fair, it's still limited by the battery capacity :stuck_out_tongue_winking_eye:

[![Youtube video](https://uavpal.com/img/yt_thumbail_github.png)](https://www.youtube.com/watch?v=e9Xl3tTwReQ)
![Disco4G softmod](https://image.ibb.co/eP6A3c/disco4glte.jpg)
![alt tag](https://user-images.githubusercontent.com/31324055/130589754-dc2da2e0-f366-434d-aad8-24c96b2bab09.png)


## Community
[![UAVPAL Slack Workspace](https://uavpal.com/img/slack.png)](https://uavpal.com/slack)

Instructions too technical? Having trouble installing the softmod? Questions on what hardware to order? Want to meet the developers? Interested in other mods (batteries, LEDs, etc.)? Interested to meet like-minded people? Having a great idea and want to let us know?\
We have a great and very active community on Slack, come [join us](https://uavpal.com/slack)!

## Why?
- simply for fun and to try to fly several Disco controller by a "master one"

## How does it work?
![High-level connection diagram](https://preview.ibb.co/c8qPP7/disco4g_highlevel_diagram_end2end.png)

In simple terms, the Wi-Fi connection is hijacked and routed via a tethering device (e.g. mobile phone) through a 4G/LTE cellular/mobile network to the Disco. As tethering device, any modern mobile phone can be used (iOS: "Personal Hotspot" or Android: "Portable WLAN hotspot").
The Disco requires a 4G/LTE USB modem to be able to send and receive data via cellular/mobile networks.


## Requirements
*Hardware:*
- [Parrot Disco](https://www.parrot.com/us/drones/parrot-disco) 
- Parrot Skycontroller 2 
- ESP32 boards to put into each disco

*Software:*
- FreeFlight Pro App on tablet/phone (can be the same device providing Wi-Fi tethering)
- LTE disco4g-parachute mod branch 1.2 installed on every Disco: https://github.com/dailyknowmore/disco4g_parachute/tree/v1.2_parachute

## Installation
to install the mod Please see Wiki article [Installation](https://github.com/uavpal/disco4g/wiki/Installation).

## How to fly  (User Manual)
to be writen

## Contact/Contribute


## Special Thanks to
- UAVPAL community :
Join our [UAVPAL Slack workspace](https://uavpal.com/slack) or check out the [issue section](https://github.com/uavpal/disco4g/issues) here on GitHub.\
Email: <img valign="bottom" src="https://image.ibb.co/mK4krx/uavpalmail2.png"> (please do not use email for issues/troubleshooting help. Join our Slack community instead!)

- Parachute mod Dailyknowmore : https://github.com/dailyknowmore

## Disclaimer
This is still a VERY EXPERIMENTAL modification! Mod and fly your Disco at YOUR OWN RISK!!!
