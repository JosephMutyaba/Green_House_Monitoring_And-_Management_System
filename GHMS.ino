#include "embedded.h"

#define relayPin 10
#define buzzerPin 12

int val = 0;
float level = 0;
float sensorValue;
float moistureValue;
float humidity;
float temperature;
int lightVal;
bool fanOnFlag = true;

const char* API_KEY = "BFYODF0LL70NPOZL";
const char* APN = "internet";
const char* USER = "";
const char* PASS = "";

const char* THINGSPEAK_HOST = "api.thingspeak.com";
const int THINGSPEAK_PORT = 80;


// static content
void setup() {
  //Added recently
  setupOLED();
  setupWaterLevel();
  setupSmokeSensor();
  setupLightSensor();
  setupBuzzerAndFan();
  setupRelay();
  //Added recently
  Serial.begin(9600);
  dht.begin();
  SIM800.begin(9600);
  Serial.println("Initializing SIM800...");
  init_gsm();
  
  //gprs_connect();
}

// dynamic content
void loop() {
  //Added Recently
  
  runWaterLevelSensor();
  smokeSensor();
  runMoisture();
  lightSensor();
  OLEDDisplay();
  delay(1000);
  actuateRelay();
  // delay(1000);

  //Added Recently
  tempAndHumidty();
  // delay(3000);
  actuateFan();
  if (!is_gprs_connected()) {
    gprs_connect();
  }
  connectToThingSpeak();
  actuateBuzzer();
}

void connectToThingSpeak(){
  // Connect to ThingSpeak
  Serial.println("Connecting to ThingSpeak...");
  SIM800.println("AT+CIPSTART=\"TCP\",\"" + String(THINGSPEAK_HOST) + "\"," + String(THINGSPEAK_PORT));
  if (waitResponse("CONNECT OK")) {
    Serial.println("Connected to ThingSpeak");
    // Prepare the data to be sent
    String data = "api_key=" + String(API_KEY)+ "&field1=" + String(temperature) + "&field2=" + String(humidity) + "&field3=" + String(level) + "&field4=" + String(moistureValue) + "&field5=" + String(sensorValue) + "&field6=" + String(lightVal);
    String postRequest = "POST /update HTTP/1.1\r\n";
    postRequest += "Host: " + String(THINGSPEAK_HOST) + "\r\n";
    postRequest += "Content-Type: application/x-www-form-urlencoded\r\n";
    postRequest += "Content-Length: " + String(data.length()) + "\r\n\r\n";
    postRequest += data;

    // Send the data to ThingSpeak
    SIM800.println("AT+CIPSEND=" + String(postRequest.length()));
    if (waitResponse(">")) {
      SIM800.println(postRequest);
      if (waitResponse("OK")) {
        Serial.println("Data sent to ThingSpeak");
      } else {
        Serial.println("Failed to send data to ThingSpeak");
      }
    } else {
      Serial.println("Error in sending data to ThingSpeak");
    }

    // Disconnect from ThingSpeak
    SIM800.println("AT+CIPCLOSE");
    waitResponse("CLOSE OK");
    Serial.println("Disconnected from ThingSpeak");
    delay(2000);
  } else {
    Serial.println("Connection to ThingSpeak failed");
  }

  // delay(5000);
}

void init_gsm() {
  SIM800.println("AT");
  waitResponse();
  delay(DELAY_MS);
  SIM800.println("AT+CPIN?");
  waitResponse("+CPIN: READY");
  delay(DELAY_MS);
  SIM800.println("AT+CFUN=1");
  waitResponse();
  delay(DELAY_MS); 
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
    SIM800.println("AT+CREG?");
  waitResponse("+CREG: 0,");
  delay(DELAY_MS);
}

void gprs_connect() {
  SIM800.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  waitResponse();
  delay(DELAY_MS);
  SIM800.println("AT+SAPBR=3,1,\"APN\",\"" + String(APN) + "\"");
  waitResponse();
  delay(DELAY_MS);
  if (USER != "") {
    SIM800.println("AT+SAPBR=3,1,\"USER\",\"" + String(USER) + "\"");
    waitResponse();
    delay(DELAY_MS);
  }
  if (PASS != "") {
    SIM800.println("AT+SAPBR=3,1,\"PWD\",\"" + String(PASS) + "\"");
    waitResponse();
    delay(DELAY_MS);
  }
  SIM800.println("AT+SAPBR=1,1");
  waitResponse("OK", 30000);
  delay(DELAY_MS);
}

boolean is_gprs_connected() {
  SIM800.println("AT+SAPBR=2,1");
  if (waitResponse("0.0.0.0") == 1) { return false; }
  return true;
}

boolean gprs_disconnect() {
  SIM800.println("AT+CGATT=0");
  waitResponse("OK", 60000);
  // delay(6000);
  return true;
}

boolean waitResponse(const char* expected_answer, unsigned int timeout) {
  unsigned long previous = millis();
  String response;
  while ((millis() - previous) < timeout) {
    while (SIM800.available()) {
      char c = SIM800.read();
      response.concat(c);
      if (response.indexOf(expected_answer) != -1) {
        Serial.println(response);
        return true;
      }
    }
  }
  Serial.println(response);
  return false;
}

//================================Other Functions==========================================
void tempAndHumidty(){
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT11 sensor!");
    return;
  }
  // print temp and humidity vakues on serial monitor
  Serial.println("Humidity Data: " + String(humidity));
  Serial.println("Temperature Data: " + String(temperature));

}
void setupWaterLevel() {
  // Set D7 as an OUTPUT
  pinMode(sensorPower, OUTPUT);

  // Set to LOW so no power flows through the sensor
  digitalWrite(sensorPower, LOW);
}

void runWaterLevelSensor() {
  level = readSensor();

  Serial.print("Water level: ");
  Serial.println(level);

  delay(1000);
}
int readSensor() {
  digitalWrite(sensorPower, HIGH);
  delay(10);
  val = analogRead(sensorPin);
  digitalWrite(sensorPower, LOW);
  return val;
}

void runMoisture() {
  moistureValue = analogRead(A4);
  Serial.print("Moisture: ");
  Serial.println(sensorValue);
  delay(100);
}
void setupSmokeSensor() {
  Serial.println("MQ2 warming up!");
  delay(20000);  // allow the MQ2 to warm up
}

void smokeSensor() {
  sensorValue = analogRead(MQ2pin);  // read analog input pin 0

  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);

  if (sensorValue > Threshold) {
    Serial.print(" | Smoke detected!");
  }

  Serial.println("");
  delay(2000);  // wait 2s for next reading
}

void setupOLED(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();
  welcomeMessage();
}
void OLEDDisplay(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("Temp    : ");
  display.println(temperature);
  display.print("Humidity: ");
  display.println(humidity);
  display.print("Water   : ");
  display.println(level);
  display.print("Moisture: ");
  display.println(moistureValue);
  display.print("Smoke   : ");
  display.println(sensorValue);
  display.print("LightInt: ");
  display.println(lightVal);
  display.display();
}

void lightSensor(){
  lightVal = analogRead(lightPin);
  Serial.print("Light Intensity: ");
  Serial.println(lightVal);
  delay(100);
}
void setupLightSensor(){
  pinMode(lightPin, INPUT);

}
void runBuzzer() {
  digitalWrite(buzzerPin,HIGH);
}
void stopBuzzer() {
  digitalWrite(buzzerPin,LOW);
}
void actuateBuzzer(){
  if (temperature < 25.0 && humidity < 60){
    runBuzzer();
        
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.print("Temperature is Very LOW!!");
    display.display();
    delay(1000);
    digitalWrite(buzzerPin,LOW);
  }
}
void setupBuzzerAndFan() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(fanPin, OUTPUT);

}
void runFan(){
  digitalWrite(fanPin, HIGH); 
  fanOnFlag = true;
}
void actuateFan(){
   if (temperature > 30.0){
    runFan();
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.print("Fan ON!!");
    display.display();}
    delay(1500);
    stopFan();
    
   }
  void stopFan(){
    if (temperature < 26.2){
      delay(2000);
      digitalWrite(fanPin, LOW);
   }
  }
  void setupRelay(){
    pinMode(relayPin, OUTPUT);
   }
   void runRelay(){
    digitalWrite(relayPin, HIGH);
   }
   void stopRelay(){
    digitalWrite(relayPin, LOW);
   }
   void actuateRelay(){
    if (lightVal < 200){
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0,0);
      display.print("Light Low!");
      display.display();
      runBuzzer();
      delay(1000);
      stopBuzzer();
      delay(1000);
      runRelay();
    }
    else{
      stopRelay();
    }
   }  
   void welcomeMessage(){
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("Welcome,  nerds!");
    display.println();
    display.print("PleaseWait");
    display.display();
   }
