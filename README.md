# ArduinoSpaceAPIPublisher

Arduino Nano + ENC28J60 = Simple SpaceAPI Publisher

## WTF?

Space API is an unified specification across hackspaces all over the world that can be used to expose information to the web. Basically it's a JSON file in a [defined](http://spaceapi.net/documentation) format which contains all kinds of (meta) data and (status) information of a hackspace (or related space in the physical world). Most importantly it contains the information whether the *door* of such a space is currently open (unlocked) or not (locked). This information can be displayed on a website or in an mobile app and is used by people who plan to visit such a space to check if people are present.

This project is the link between a physical *door* and the mentioned *Space API JSON file*. It does so by continuously monitoring a sensor attached to the door and reporting its status (open or not) to a server (in our case spaceapi.net) which is hosting the JSON file.

In more detail, the software in this repo runs on an cheep *Arduino Nano* which is connected to the network via an *ENC28J60* module (see wiring below). Furthermore the Arduino is connected to the sensor attached to the door. The software is communicating to the spaceapi.net API via simple HTTP. Furthermore, some debugging information is transmitted via the serial interface.

Check out [the SpaceAPI status produced by this project](https://spaceapi.net/new/space/losfuzzyslab/status) (also available [as json](https://spaceapi.net/new/space/losfuzzyslab/status/json)).


## Case

The case has been made with [realraum's Lasercutter](https://realraum.at/).

**TODO**


## Wiring

The Arduino Nano* and the *ENC28J60* network module are wired in the following way:

![Wiring](https://raw.githubusercontent.com/stefan2904/ArduinoSpaceAPIPublisher/master/pictures/wiring.png)


## Used hardware

* Arduino Nano
* ENC28J60 (Ethernet controller)
* **TODO:** some doorsensor


## Used software

* Arduino IDE 1.6.8: https://www.arduino.cc/en/Main/Software
* EtherCard driver: https://github.com/jcw/ethercard
* spaceapi.net API (and hosted endpoint)
