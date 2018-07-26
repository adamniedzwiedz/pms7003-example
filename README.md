# Mongoose OS the PMS7003 dust sensor example (C/C++)

For the details look at the [library](https://github.com/adamniedzwiedz/pms7003-mongoose-os-lib)

## Typical connection.

<p align="center">
  <img src="https://s3-eu-west-1.amazonaws.com/github-an/pms7003_connection.png">
</p>

## Overview

This example shows how to read the PM concentration (air polution) using the PMS7003 dust sensor with ESP8266 and Mongoose OS.

Since communication with the sensor is done via UART0 the debug messages are sent through UART1.

In the example the PMS7003 is set to work in a *passive mode* and readings are done via *requests*. However it is possible to set the sensor in an *active mode*  `pms7003_set_mode(PMS7003_ACTIVE_MODE)` or put it to sleep `pms7003_sleep()` and then wake it up `pms7003_wakeup()`. 

_Note_

The first command must be sent with at least 2 second delay after power on the sensor.

## How to install this app
- [Download](https://github.com/adamniedzwiedz/pms7003-mongoose-example/archive/master.zip) or clone `git clone https://github.com/adamniedzwiedz/pms7003-mongoose-example` the project
- Install [mos tool](https://mongoose-os.com/software.html)
- Open command line window and run `mos build --arch esp8266`
- Run `mos flash`

