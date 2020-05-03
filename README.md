# Datacake MQTT

This repository shows an implementation to connect an ESP32 to the [Datacake](https://datacake.co/) MQTT Broker.

## Setup
### Hardware
- Connect a pushbutton to ESP32 Pin 34 with a pull-down resitor
- Connect a RGB LED to pins R=33, G=32, B=35

### Datacake Cloud
Create a new device and add one Field with identifier `BUTTONVALUE` and type `Boolean`.

### Software
The project was made with [PlatfromIO](https://platformio.org/), but you can still use it with the Arduino IDE. If you want to use the Arduino IDE make sure you have the [PubSubClient](https://github.com/knolleary/pubsubclient/) library installed.
- make a copy of `src/secrets_template.h` to `secrets.h`
- Fill in your WiFi credentials
- Get your [Access Token](https://docs.datacake.de/mqtt/mqtt) and place it into `MQTT_USER` and `MQTT_PASS`
- Get your MQTT Topics as described in the datacake [docs](https://docs.datacake.de/mqtt/mqtt#topic-structure) and place them into `MQTT_SUB_TOPIC` and `MQTT_PUB_TOPIC`
  - make sure you append `/BUTTONVALUE` to the end of these topics
