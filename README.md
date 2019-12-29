# Teleinfo 2 mqtt

This directory contains:

- serialStubUno for Arduino Uno, to simulate a basic Tempo teleinfo frame
- teleinfo for MKR 1010, to read teleinfo data from RX and send them via MQTT

Before compiling teleinfo, add ```arduino_secrets.h``` with the following content:

```h
#define SECRET_SSID "XXX"
#define SECRET_PASS "XXX"
#define SECRET_MQTT_USER "XXX"
#define SECRET_MQTT_PASS "XXX"
```
