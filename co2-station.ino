#include <ThingerESP8266.h>
#include <Adafruit_BME280.h>
#include "MHZ19.h"                                        
#include <SoftwareSerial.h>

//LEDS
#define GREEN D1
#define YELLOW D2
#define RED D3

// WIFI
#define WLAN_SSID ""
#define WLAN_PASS ""

//Thinger.io
#define THINGERIO_USERNAME ""
#define THINGERIO_DEVICE_ID ""
#define THINGERIO_DEVICE_CREDENTIAL ""
ThingerESP8266 thing(THINGERIO_USERNAME, THINGERIO_DEVICE_ID, THINGERIO_DEVICE_CREDENTIAL);

// BME280
#define I2C_SDA D6
#define I2C_SCL D5
TwoWire I2CBME = TwoWire();
Adafruit_BME280 bme;

// MHZ19
#define MH_Z19_RX D7
#define MH_Z19_TX D4
MHZ19 mMHZ19;
SoftwareSerial co2Serial(MH_Z19_RX, MH_Z19_TX);
#define CO2THRESHOLD_YELLOW     800
#define CO2THRESHOLD_RED        1000

//Alarm
#define CO2THRESHOLD_ALARM        CO2THRESHOLD_RED
#define CO2THRESHOLD_ALARM_HYST   CO2THRESHOLD_RED * 0.9
bool alarmUp;

void setup() {
  Serial.begin(9600);
  Serial.println();

  //BME280
  Serial.println("BME280");
  I2CBME.begin(I2C_SDA, I2C_SCL, 100000);
  unsigned status = bme.begin(0x76, &I2CBME);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(), 16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(1000);
  }
  else
  {
    Serial.println("BME280 Sensor detected!");
  }

  //MHZ19
  Serial.println("MHZ 19B");
  co2Serial.begin(9600);
  mMHZ19.begin(co2Serial);
  mMHZ19.autoCalibration();

  //Thinger.io
  thing.add_wifi(WLAN_SSID, WLAN_PASS);
  thing["temperature"] >> outputValue(bme.readTemperature());
  thing["pressure"] >> outputValue(bme.readPressure() / 100.0F);
  thing["humidity"] >> outputValue(bme.readHumidity());
  thing["co2"] >> outputValue(mMHZ19.getCO2());
  thing["temperature2"] >> outputValue(mMHZ19.getTemperature());

  //LEDs
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(RED, OUTPUT);
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, LOW);

  //Alarm
  alarmUp = false;

  //MHZ19 PREHEAT
  int led = 0;
  while(millis() < 90000) {
    if(led == 0) {
      digitalWrite(GREEN, HIGH);
      digitalWrite(YELLOW, HIGH);
      digitalWrite(RED, LOW);
    }
    else if(led == 1) {
      digitalWrite(GREEN, LOW);
      digitalWrite(YELLOW, HIGH);
      digitalWrite(RED, HIGH);
    }
    else if(led == 2) {
      digitalWrite(GREEN, HIGH);
      digitalWrite(YELLOW, LOW);
      digitalWrite(RED, HIGH);
    }
    led = (led + 1) % 3;
    delay(2000);
  }
}

void loop() {
  //LEDs
  if(millis()%5000 == 0){
    int co2 = mMHZ19.getCO2();
    if (co2 > 0) {
      Serial.print("CO2 level: ");
      Serial.print(co2);
      Serial.print(" [");
      if(co2 < CO2THRESHOLD_YELLOW) {
        Serial.print("GREEN");
        digitalWrite(YELLOW, LOW);
        digitalWrite(RED, LOW);
        digitalWrite(GREEN, HIGH);
      }
      else if(co2 < CO2THRESHOLD_RED) {
        Serial.print("YELLOW");
        digitalWrite(GREEN, LOW);
        digitalWrite(RED, LOW);
        digitalWrite(YELLOW, HIGH);
      }
      else {
        Serial.print("RED");
        digitalWrite(GREEN, LOW);
        digitalWrite(YELLOW, LOW);
        digitalWrite(RED, HIGH);
      }
      Serial.println("]");

      if(!alarmUp && co2 >= CO2THRESHOLD_ALARM) {
        pson data;
        data["co2"] = co2;
        thing.call_endpoint("weatherstation", data);
        alarmUp = true;
      }
      else if (alarmUp && co2 < CO2THRESHOLD_ALARM_HYST) {
        alarmUp = false;
      }
    }
    
    Serial.print("Temperature: ");
    Serial.println(bme.readTemperature());
  }
  
  //Thinger.io
  thing.handle();
}
