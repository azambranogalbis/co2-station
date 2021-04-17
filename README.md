# co2-station
A home CO2 and temperature station with NODEMCU and thinger.io.

This project implements:
- A semaphore (using three colored LEDs) that will signal the current level of CO2 in the room in which it is installed
- All measurements are regularly updated in [thinger.io](https://thinger.io/), where you can use it to build your own dashboards
- High CO2 levels detection calls an endpoint in [thinger.io](https://thinger.io/), which you can use to trigger further actions

Overall cost of the required components is around 20€.

## Hardware
### Required hardware
- [NODEMCU development board](https://www.nodemcu.com/index_en.html)
- [MH-Z19 CO2 sensor](https://www.winsen-sensor.com/d/files/PDF/Infrared%20Gas%20Sensor/NDIR%20CO2%20SENSOR/MH-Z19%20CO2%20Ver1.0.pdf)
- [BME280 temperature, humidity and pressure sensor](https://cdn-shop.adafruit.com/datasheets/BST-BME280_DS001-10.pdf)
- Red. yellow and green LEDs
- 1kohm resistance (or similar order of magnitude)

### Hardware connections
All elements need to be connected to the NODEMCU board as follows:

|Element|Element Pin|NODEMCU Pin|
--- | --- | ---
|BME280|Vin|3v3|
|BME280|GND|GND|
|BME280|SCL|D5|
|BME280|SDA|D6|
|MH-Z19|Vin|Vin|
|MH-Z19|GND|GND|
|MH-Z19|TX|D7|
|MH-Z19|RX|D4|
|Red LED|Anode|D3|
|Red LED|Cathode|Resistance (side A)|
|Yellow LED|Anode|D2|
|Yellow LED|Cathode|Resistance (side A)|
|Green LED|Anode|D1|
|Green LED|Cathode|Resistance (side A)|
|Resistance|Side A|All three LED cathodes|
|Resistance|Side B|GND|

## Deployment notes
This sketch can be deployed in the NODEMCU development board using [Arduino IDE](https://create.arduino.cc/projecthub/najad/using-arduino-ide-to-program-nodemcu-33e899)
Some extra libraries will be needed (downloaded directly from the Arduino IDE):
- [Adafruit BME280 lib](https://github.com/adafruit/Adafruit_BME280_Library)
- [MH-Z19](https://github.com/WifWaf/MH-Z19)
- [thinger.io](https://github.com/thinger-io/Arduino-Library)


## Thinger.io integration
This sketch uploads measurements to the [thinger.io](https://thinger.io/) platform as a *device* element. In order to get ready, you will need to:
- Create an account (from which you will get an *username*)
- [Create a new device](https://docs.thinger.io/features/devices-administration) in your account (from which you will get a *device_id* and *device_credential*)
- [Create an endpoint](https://docs.thinger.io/features/endpoints-1) in your account, which will get fired whenever measured CO2 levels go too high (it can be used to trigger further actions, such as sending an alert to [a chat in Telegram](https://docs.thinger.io/features/endpoints-1#telegram-bot-endpoint)) 

Once up and running, the created device will be populated with the following measurements:
- **temperature**: temperature (ºC) measurements from BME280 sensor
- **pressure**: pressure (mbar) measurements from BME280 sensor
- **humidity**: humidity (%) measurements from BME280 sensor
- **co2**: co2 (ppm) measurements from MH-Z19 sensor
- **temperature2**: temperature (ºC) measurements from MH-Z19 sensor (even though those are not accurate)

## Required preparation in the sketch
Prior to deploying this sketch, you will need to complete the following sections:
- **WiFi**: fill *WLAN_SSID* and *WLAN_PASS* with the SSID and password of your own WiFi connection
- **Thinger.io**: fill *THINGERIO_USERNAME*, *THINGERIO_DEVICE_ID*, *THINGERIO_DEVICE_CREDENTIAL* and *THINGERIO_ENDPOINT* with the corresponding information of your [thinger.io](https://thinger.io/) account
