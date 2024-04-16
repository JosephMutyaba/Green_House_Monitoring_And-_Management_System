
#include <Adafruit_Sensor.h>

#define Threshold 400
#define MQ2pin A1
#define sensorPower 7
#define sensorPin A3
#define lightPin A0
#define fanPin 32
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#include <DHT.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define DHTPIN 4       // Digital pin connected to the DHT sensor

#define DHTTYPE DHT11  // DHT 11

#define DELAY_MS 1000

#define DHT11_PIN A2
#define DHTTYPE DHT11

#define rxPin 52
#define txPin 53

DHT dht(DHT11_PIN, DHTTYPE);

//prototypes
void init_gsm();
void gprs_connect();
boolean gprs_disconnect();
boolean is_gprs_connected();
boolean waitResponse(const char* expected_answer = "OK", unsigned int timeout = 2000);

//Other Prototypes
void setupWaterLevel();
void runWaterLevelSensor();
int readSensor();
void runMoisture();
void setupSmokeSensor();
void smokeSensor();
void setupOLED();
void OLEDDisplay();
void lightSensor();
void setupLightSensor();
void connectToThingSpeak();
void runBuzzer();
void actuateBuzzer();
void stopFan();
void actuateFan();
void runFan();
void setupBuzzerAndFan();
void setupRelay();
void actuateRelay();
void stopRelay();
void runRelay();
void welcomeMessage();
void stopBuzzer();
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
SoftwareSerial SIM800(rxPin, txPin);

