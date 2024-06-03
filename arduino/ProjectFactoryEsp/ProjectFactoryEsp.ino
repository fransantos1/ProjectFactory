#include "LiquidCrystal_I2C.h"
#include "DHT.h"
#include "AiEsp32RotaryEncoder.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "FS.h"
#include "SD.h"
#include <SPI.h>
#include "time.h"
#include <EEPROM.h>
#include <WebServer.h>
#define TOKEN_ADDRESS 0
#define EEPROM_SIZE 100
#define DHTPIN 4
#define DHTTYPE DHT22

#define PIN_RED 17
#define PIN_GREEN 16
#define PIN_BLUE 2
#define PIN_BUZZER 27
#define PIN_LED 14
#define PIN_BTN_EMERGENCY 26
#define ROTARY_ENCODER_A_PIN 35
#define ROTARY_ENCODER_B_PIN 34
#define ROTARY_ENCODER_BUTTON_PIN 25
#define ROTARY_ENCODER_STEPS 2
#define APITOKEN_LEN 64

// TODO ORGANIZE CODE
// TODO diiferent EMERGENCY CODES
// TODO screen feedback when sending data to the api
// TODO NFC AUTHENHICATION
// TODO CORRECT MENU, when going back reset the menu itself to overwrite characters

// TODO LIGHTSHOW Function
// TODO timeout when the net is not available and try to connect every 5 s or smth
// TODO error when connecting to the server and make the same timeout has the net
// TODO HEARTBEAT TO API




#define API_KEY "134a8fb6-3824-497a-a23c-1b89abe03a8a"

String serverName = "http://192.168.43.47:8080/";

const char* ssid = "esp32wifi";
const char* password = "espfrancisco";
//const char* ssid = "MEO-52EE71";
//const char* password = "7A41701C96";
char apiToken[APITOKEN_LEN + 1];
WebServer server(80);
//DHT SENSOR


// FLAGS 


bool isEmergency = false; // if the tent is in an emergency
bool isAuth = false; // if the esp is authenthicated
bool isConn = false; //if the esp is connected with wifi




const char* filename = "/data.bin";
DHT dht(DHTPIN, DHTTYPE);
int lcdHeight = 4;
int lcdWidth = 20;
LiquidCrystal_I2C lcd(0x27, lcdWidth, lcdHeight);
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);

void IRAM_ATTR readEncoderISR() {
  rotaryEncoder.readEncoder_ISR();
}
void setupRotaryControl() {
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.disableAcceleration();  //acceleration is now enabled by default - disable if you dont need it
}
void setupDHTsensor() {
  dht.begin();
}
void setupDisplay() {
  lcd.init();
  lcd.backlight();
}


//
// sync esp32 time with real time, so it can store sensor data and know its date/time without sending it to the api
//

unsigned long lastEpochUpdatedTime = 0;
unsigned long epochTimerDelay = 10 * 60 * 1000;  //10 minutes
const char* ntpServer = "pool.ntp.org";
unsigned long oldEpochTime;
unsigned long epochTime;
bool firstStart = true;
unsigned long getTime() {/////////////////////////////////////////////////////////////AFFECTED BY CONN
  while (true) {
    unsigned long currentCpuTime = millis();
    if ((currentCpuTime - lastEpochUpdatedTime) > epochTimerDelay || firstStart) {
      firstStart = false;
      time_t now;
      struct tm timeinfo;
      if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        continue;
      }
      time(&now);
      lastEpochUpdatedTime = millis();
      oldEpochTime = now;
      return now;
    }
    unsigned long elapsedCpuTime = (currentCpuTime - lastEpochUpdatedTime) / 1000;
    unsigned long currentEpochTime = oldEpochTime + elapsedCpuTime;
    return currentEpochTime;
  }
}


void setupWifi() {/////////////////////////////////////////////////////////////AFFECTED BY CONN
  WiFi.begin(ssid, password);
  lcd.clear();
  lcd.setCursor(0, 0);  //Set cursor to character 0 line 1
  lcd.print("Connecting");
  int n = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if(n> 10){
      lcd.clear();
      lcd.setCursor(2, 1);
      lcd.print("falied to connect");
      delay(1000);
      return;
    }
    delay(500);
    lcd.setCursor(n + 10, 0);
    lcd.print(".");
    n++;
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connected WiFi_IP: ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  server.begin();
  delay(1000);
}





//
// Sensor data LOGGING
// 

//easy to add more sensors
struct SensorData {
  unsigned long timestamp;
  float humidity;
  float temperature;
};
unsigned long lastUpdateDataTime = 0;  // Variable to store the last time the temperature and humidity were updated
int originaldataReadingDelay = 1000;  
int dataReadingDelay = originaldataReadingDelay;            // Delay between updates in miliseconds
int dataBufferIndex = 0;
const int dataBufferSize = 10;
SensorData sensorDataBuffer[dataBufferSize];

int numberOfBuffersInSD = 5; //Number of Buffers saved in SD before sending

float maxTemp = 31;

// Read sensor data and add it to a buffer
void sensorDataController() {  
  unsigned long currentTime = millis();  // Get the current time



  if (currentTime - lastUpdateDataTime >= dataReadingDelay) {
    if (!SD.begin()) {
      //DEBUGGER TO KEEP TRACK OF BUFFER
      Serial.print("Track Buffer: ");
      Serial.println(dataBufferIndex);
    }

    lastUpdateDataTime = currentTime;
    sensorDataBuffer[dataBufferIndex].humidity = dht.readHumidity();
    sensorDataBuffer[dataBufferIndex].temperature = dht.readTemperature();
    sensorDataBuffer[dataBufferIndex].timestamp = getTime();
    dataBufferIndex++;
    if (dataBufferIndex >= dataBufferSize) {
      dataBufferIndex = 0;
      if (!SD.begin()) {
        //change dataReadingDelay to change the sensorDataBuffer[] instead of the information of the SDCard, but at the same interval
        dataReadingDelay = originaldataReadingDelay*numberOfBuffersInSD;//sends the data at the same interval independentally if the sdCard is mounted or not
        sendSensorDataFromBuffer();
        Serial.println("SD card not available");
        return;
      }
      dataReadingDelay = originaldataReadingDelay;
      //if SDCard 
      writeSensorDataToFile();
    }
  }
}

// Write from the buffer to the SDCard

void writeSensorDataToFile() {
  static size_t lastSize;
  size_t maxSDFileSize = sizeof(struct SensorData) * dataBufferSize * numberOfBuffersInSD;
  File file = SD.open(filename, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for writing.");
    return;
  }
  for (int i = 0; i < dataBufferSize; i++) {
    Serial.print(String(file.write((uint8_t*)&sensorDataBuffer[i], sizeof(SensorData))) + " | ");
  }
  Serial.println("");
  size_t fileSize = file.size();
  file.close();
  Serial.println(fileSize);
  if (lastSize == fileSize) {
    //SD.remove(filename);
  }
  if (fileSize >= maxSDFileSize) {
    sendSensorDataFromSD();
  }
  lastSize = fileSize;
}
// Remove the data from de sensorDataBuffer[] and send it to the API
void sendSensorDataFromBuffer() {
  char* payload = new char[sizeof(SensorData) * dataBufferSize*10];
  strcpy(payload, "[");
  for(int i = 0; i < dataBufferSize; i++) {
    SensorData data = sensorDataBuffer[i];
    char entry[sizeof(SensorData) * 10];
    sprintf(entry, "{\"timestamp\": %lu, \"humidity\": %.2f, \"temperature\": %.2f}", data.timestamp, data.humidity, data.temperature);
    strcat(payload, entry);
    if (i < dataBufferSize - 1) {
      strcat(payload, ",");
    }
  }
  strcat(payload, "]");
  sendData(payload);
  delete[] payload;
}



// Remove the data from de SD and send it to the API
void sendSensorDataFromSD() {
  File file = SD.open(filename, FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for reading.");
    return;
  }
  size_t filesize = file.size();
  Serial.println("Size: " + String(filesize));
  Serial.println("N of Data Structures: " + String(filesize / 12));
  unsigned long stopwatch = millis();
  char* payload = new char[filesize * 10];
  Serial.println("Created payload");
  strcpy(payload, "[");
  size_t entrySize = sizeof(SensorData);
  size_t numEntries = filesize / entrySize;
  for (size_t i = 0; i < numEntries; i++) {
    uint8_t buffer[sizeof(SensorData)];
    file.read(buffer, sizeof(SensorData));
    SensorData data = *((SensorData*)buffer);
    char entry[sizeof(SensorData) * 10];
    sprintf(entry, "{\"timestamp\": %lu, \"humidity\": %.2f, \"temperature\": %.2f}", data.timestamp, data.humidity, data.temperature);
    strcat(payload, entry);
    if (i < numEntries - 1) {
      strcat(payload, ",");
    }
  }
  file.close();
  strcat(payload, "]");
  sendData(payload);
  delete[] payload;

  file = SD.open(filename, FILE_WRITE);
  if (file) {
    file.close(); // Closing immediately truncates the file to zero length
    Serial.println("File emptied successfully.");
  } else {
    Serial.println("Failed to open file for truncating.");
  }
  Serial.println("It Took: " + String((millis() - stopwatch)) + " ms");
}

//
//---------------------------------SDCARD SETUP
//
void setupSDCard() {
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

//
//---------------------------CONNECTION
//
void connectionHandler(){
  int interval = 10000;
  static unsigned long current;  
  static unsigned long last; 
  current = millis();
  if (current - last > interval)
  {
      bool connectionStatus = (WiFi.status() == WL_CONNECTED);
      if(!connectionStatus){
          //NO WIFI
      }
      if(connectionStatus && !isAuth || !isConn){
          authenticateWithServer();
          //NOT AUTH
      }
    last = current;
  }
}
//
//---------------------------AUTHENTHICATION---------------------------------------------------------------------
//

//Saves the api token to the EEPROM

void saveTokenEEPROM(const char* token) {
  int length = strlen(token);
  for (int i = 0; i < length; i++) {
    EEPROM.write(TOKEN_ADDRESS + i, token[i]);
  }
  EEPROM.write(TOKEN_ADDRESS + length, '\0');
  EEPROM.commit();
  Serial.println(readTokenEEPROM());
}
String readTokenEEPROM() {
  String token = "";
  char ch;
  int i = 0;
  while ((ch = EEPROM.read(TOKEN_ADDRESS + i)) != '\0') {
    token += ch;
    i++;
  }
  return token;
}
//GENERATE A TOKEN TO THE API TO TALK TO THE ESP
void genApiToken() {
  static const char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  static const int charactersLength = strlen(characters);
  for (int i = 0; i < APITOKEN_LEN; i++) {
    apiToken[i] = characters[random(charactersLength)];
  }
  apiToken[APITOKEN_LEN] = '\0';
}
String serverAPICheckToken() {
  HTTPClient http;
  genApiToken();//GEN A TOKEN 
  http.begin((serverName + "api/node/authenticate").c_str());  //THIS REQUEST WILL EITHER RETURN THE NODE TOKEN OR NOTHING
  http.addHeader("Content-Type", "application/json");
  http.addHeader("macaddress", WiFi.macAddress());  //send Macaddress
  http.addHeader("auth_token", readTokenEEPROM());  //send Token if it has one
  http.addHeader("apiToken", String(apiToken));     //send token to access esp32 api
  http.addHeader("ip", WiFi.localIP().toString());  //send ipAddress
  http.addHeader("api_key", API_KEY);
  int httpResponseCode = http.GET();
  if (httpResponseCode == 200) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println("Payload: " + payload);
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
      Serial.print("Failed to parse JSON: ");
      Serial.println(error.c_str());
      return "Error";
    }
    //VERIFY THE USERTOKEN and if true than send a signal to the arduino
    const char* token = doc["result"]["token"];
    if (String(token) == "") {
      Serial.println("CorrectToken");
    } else {
      saveTokenEEPROM(token);
    }
    Serial.println(String(token));
    const char* userToken = doc["result"]["userToken"];
    if (String(userToken) == "") {
      Serial.println("Has a Owner");
    } else {
      //handle token
      Serial.print('\0');
      Serial.print("NFC");
      Serial.print('\0');
      Serial.print(userToken);
      Serial.print('\0');
    }
    isAuth = true;
    isConn = true;
    return String(token);
  } else {
    Serial.print("Error code: ");//-1 couldnt connect
    if(httpResponseCode == -1){
      isConn = false;
      Serial.print("coudnt access api: ");
    }else{
      isConn = true;
    }
    isAuth = false;
    Serial.println(httpResponseCode);
    return "Error";
  }
  http.end();
}
void authenticateWithServer() {/////////////////////////////////////////////////////////////AFFECTED BY CONN
  String token = readTokenEEPROM();
  token = "";
  String response = serverAPICheckToken();
}

//
//---------------------------------------LedController------------------------------------
//
void ledController(int r, int g, int b){
    analogWrite(PIN_RED, r);
    analogWrite(PIN_GREEN, g);
    analogWrite(PIN_BLUE, b);
}
void resetLed(){
    analogWrite(PIN_RED, 0);
    analogWrite(PIN_GREEN, 0);
    analogWrite(PIN_BLUE, 0);
}

//
//---------------------------------------ESPAPI---------------------------------------
//

void setupAPI(){
  server.on("/setRGB", HTTP_POST, LightShowAPI);//RGB
  server.on("/setLed", HTTP_POST, controlLedAPI);//Ambient
  server.on("/locate", HTTP_POST, locateAPI);
  server.on("/reset", HTTP_POST, ResetUserAPI);
}



bool isLocate = false;

void locateAPI() {
  String body = server.arg("plain");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, body);
  if(isEmergency){
    server.send(403, "application/json", "{ \"msg\": \"Node in Emergency Mode\" }");
    return;
  }
  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
  }
  String token = doc["token"];
  if (token.equals("null") || !token.equals(String(apiToken))) {
    server.send(401, "application/json", "{ \"msg\": \"Not Authorized\" }");
    return;
  }
  Serial.println("LOcate");
  isLocate = !isLocate; 
  Serial.println(isLocate);
  if(!isLocate){
    resetLed();
  }

  // Respond to the client
  server.send(200, "application/json", "{}");

}
void ResetUserAPI() {
  // Send a respons
  //verifications needed
  //needs TokenAPI
  String body = server.arg("plain");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, body);
  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
  }
  String token = doc["token"];
  if (token.equals("null") || !token.equals(String(apiToken))) {
    server.send(401, "application/json", "{ \"msg\": \"Not Authorized\" }");
    return;
  }
  const char* userToken = doc["userToken"];
  if (String(userToken) != "") {
    //handle token
    Serial.print('\0');
    Serial.print("NFC");
    Serial.print('\0');
    Serial.print(userToken);
    Serial.print('\0');
  }
  // Respond to the client
  server.send(200, "application/json", "{}");
}






//Controll RGB
void LightShowAPI() {
  // Send a respons
  //verifications needed
  //needs TokenAPI
  static bool activate;
  String body = server.arg("plain");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, body);
  if(isEmergency){
    server.send(403, "application/json", "{ \"msg\": \"Node in Emergency Mode\" }");
    return;
  }
  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
  }
  String token = doc["token"];
  if (token.equals("null") || !token.equals(String(apiToken))) {
    server.send(401, "application/json", "{ \"msg\": \"Not Authorized\" }");
    return;
  }
  activate = doc["activate"];
  Serial.println(body);
  int red = doc["value"]["red"];
  int green = doc["value"]["green"];
  int blue = doc["value"]["blue"];
  if (activate) {
    ledController(doc["value"]["red"], doc["value"]["green"],doc["value"]["blue"]);
  } else {
    resetLed();
  }
  // Respond to the client
  server.send(200, "application/json", "{}");
  
}

bool ambientLight = false;
//Controll Ambient light
void controlLedAPI() {
  String body = server.arg("plain");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, body);
  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
  }
  String token = doc["token"];
  if (token.equals("null") || !token.equals(String(apiToken))) {
    server.send(401, "application/json", "{ \"msg\": \"Not Authorized\" }");
    return;
  }
  ambientLight = !ambientLight; 
  if(!ambientLight){
    digitalWrite(PIN_LED, LOW);
  }else{
    digitalWrite(PIN_LED, HIGH);
  }
  // Respond to the client
  server.send(200, "application/json", "{}");
  
}
bool isMenuChanged = true;
///
///------------------------------EMERGENCY---------------------
///

bool emergencyChange = false;
///////////////////////////////////////////////////////////////////////////NEEDS AUTH
void emergencyRequest(bool Emergency){/////////////////////////////////////////////////////////////AFFECTED BY CONN
  if(!isAuth)
    return;
  HTTPClient http;
  // Specify content-type header
  http.begin(serverName + "api/node/emergency");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("auth_token", readTokenEEPROM());
  char payload[50];
  sprintf(payload, "{\"isEmergency\": %s}", Emergency ? "true" : "false");
  int httpResponseCode = http.POST(String(payload));

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    // Print response body
    String response = http.getString();
    Serial.println("Response: " + response);
  } else {
    Serial.println("Error during POST request.");
  }

  // Close connection
  http.end();
}
void emergencyInterrupt() { 
  static int counter;
  int interval = 250;//ms
  static unsigned long button_time;  
  static unsigned long last_button_time; 
  button_time = millis();
  if (button_time - last_button_time > interval)
  {
        if(isEmergency){
          counter++;
          if(counter == 3){
            isMenuChanged = true;
            isEmergency =false;
            emergencyChange= true;
            counter = 0;
          }
        }else{
          emergencyChange = true;
          isEmergency = true;
          isMenuChanged = true;
        }
        last_button_time = button_time;
  }
} 
void Emergency(){
    static bool isOn = false;
    int interval = 500;//ms
    static unsigned long button_time;  
    static unsigned long last_button_time; 
    button_time = millis();
    if (button_time - last_button_time > interval)
    {
      last_button_time = button_time;
      if(isOn){
        analogWrite(PIN_BUZZER, 125);
        ledController(255,0,0);
        
        isOn = false;
        
      }else{
        isOn=true;
        analogWrite(PIN_BUZZER, 255);
        ledController(0,0,0);
      }
       
    }
}
bool emergencyHandler(){
  if(emergencyChange){
    emergencyChange = false;

    if(!isEmergency)
      analogWrite(PIN_BUZZER, 0);
    resetLed();
    emergencyRequest(isEmergency);
  }
  if(isEmergency){
      Emergency();
      return true;
  }
  return false;
}
//
//----------------------------LocateNodeHandler--------------------
//
void locateController(){
  if(isEmergency || !isLocate){
    return;
  }

  static bool isOn = false;
  int interval = 500;//ms
  static unsigned long button_time;  
  static unsigned long last_button_time; 
  button_time = millis();
  if (button_time - last_button_time > interval)
  {
    last_button_time = button_time;
    if(isOn){
      ledController(255,255,0); //yellow
      isOn = false;
    }else{
      isOn=true;
      ledController(0,0,0);
    }
      
  }


}

//
//-----------------------------SETUP/LOOP------------------------------
//


void setup() {
  pinMode(PIN_LED, OUTPUT);
  EEPROM.begin(EEPROM_SIZE);
  Serial.begin(115200);
  setupDisplay();
  setupWifi();
  setupAPI();
  authenticateWithServer();
  configTime(0, 0, ntpServer);
  setupDHTsensor();
  setupSDCard();
  setupRotaryControl();

  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_BTN_EMERGENCY, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  attachInterrupt(PIN_BTN_EMERGENCY, emergencyInterrupt, RISING);
}
long max_delay = 0;
void loop() {
  unsigned long stopwatch = millis();

  
  connectionHandler();
  ControlMenu();
  server.handleClient();/////////////////////////////////////////////////////////////AFFECTED BY CONN
  locateController();
  if(emergencyHandler()){ 
    return;
  }
 
  sensorDataController();
  if((millis() - stopwatch) > max_delay){
    max_delay = (millis() - stopwatch);
    Serial.println("It Took: " + String(max_delay) + " ms");
  }
   

}
//
//----------------------------------------------------GENERAL HTTP REQUESTS
//
String httpGetRequest_Concert() {/////////////////////////////////////////////////////////////AFFECTED BY CONN
  if(!isConn)
    return "NaN";

  String payload;
  HTTPClient http;
  String serverPath = serverName + "api/concerts/get";
  http.begin(serverPath.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    payload = http.getString();
  } else {
    Serial.print("Error code: ");//-1 couldnt connect
    if(httpResponseCode == -1){
      isConn = false;
      isAuth = false;
    }
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    return "NaN";
  }
  http.end();
  
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
    return "Error";
  }
  const char* band = doc["concert"]["band"];
  const char* time = doc["concert"]["time"];
  return String(band) + " " + String(time);
}
///////////////////////////////////////////////////////////////////////////NEEDS AUTH
void sendData(char* payload) {/////////////////////////////////////////////////////////////AFFECTED BY CONN
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Sending");
  if(!isAuth){
    lcd.clear();
    isMenuChanged = true;
    return;
  }
  HTTPClient http;

  // Specify content-type header
  http.begin(serverName + "api/node/data");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("auth_token", readTokenEEPROM());

  // Send POST request and get response
  int httpResponseCode = http.POST(String(payload));

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    // Print response body
    String response = http.getString();
    Serial.println("Response: " + response);
  } else {
    Serial.print("Error code: ");//-1 couldnt connect
    if(httpResponseCode == -1){
      isConn = false;
      isAuth = false;
    }else if(httpResponseCode == 401){
      isAuth = false;
    }
    Serial.print("coudnt access api: ");
    Serial.println(httpResponseCode);
  }
  lcd.clear();
  isMenuChanged = true;
  // Close connection
  http.end();
}


//
//------------------------MENU---------------------------------
//
void rotaryController(int minOption, int maxOption) {  //minOption, on lcd first option line, Max, lcd last option line
  if (rotaryEncoder.encoderChanged()) {
    for (int i = minOption; i <= maxOption; i++) {
      lcd.setCursor(0, i);  //Set cursor to character 0 line 1
      lcd.print(" ");
    }
    lcd.setCursor(0, rotaryEncoder.readEncoder() + minOption);
    lcd.print(">");
  }
}

int MenuIndex = 0;    // 0 = menu / 1 = temphumDisplay / 2 = ConcertDisplay
int optionIndex = 0;  //What option is selected
int titlePosition = 2;
void ControlMenu() {
  if(isEmergency){
    if(isMenuChanged){
      isMenuChanged = false;
      lcd.clear();
      lcd.setCursor(2, 1);
      lcd.print("EMERGENCY");
    }
    return;
  }
  switch (MenuIndex) {
    case 0:
      controlMainMenu();
      break;
    case 1:
      controlTemHum();
      break;
    case 2:
      controlDisplayConcert();
      break;
    case 3:
      controlDisplayConcert();
      break;
  }
}
void controlMainMenu() {
  if (isMenuChanged) {                        //INITIALIZATION
    rotaryEncoder.setBoundaries(0, 2, true);  // min value, max value(Number of options on menu), loop back(after 2 comes back to 0)
    lcd.clear();
    lcd.setCursor(titlePosition, 0);  //Set cursor to character 2 on line 0
    lcd.print("MENU");
    lcd.setCursor(2, 1);  //Set cursor to character 2 on line 0
    lcd.print("Temperatura & Hum");
    lcd.setCursor(0, 1);  //Set cursor to character 2 on line 0
    lcd.print(">");
    lcd.setCursor(2, 2);  //Set cursor to character 2 on line 0
    lcd.print("Concertos");
    lcd.setCursor(2, 3);  //Set cursor to character 2 on line 0
    lcd.print("Connections");
    MenuIndex = 0;
    isMenuChanged = false;
  }
  rotaryController(1, 3);
  //Option Controller
  if (rotaryEncoder.isEncoderButtonClicked()) {
    switch (rotaryEncoder.readEncoder()) {
      case 0:  //Temperatura e humidade
        isMenuChanged = true;
        controlTemHum();
        break;
      case 1:  //Concertos
        isMenuChanged = true;
        controlDisplayConcert();
        break;
      case 2:  //Connections
        isMenuChanged = true;
        controlDisplayConnections();
        Serial.println("Connection");
        break;
    }
  }
}
unsigned long lastUpdateConcertTime = 0;
unsigned long lastConcertScrollTime = 0;
void controlDisplayConcert() {
  int ConcertRequestDelay = 30000;  //30 seconds
  int ScrollDelay = 500;
  String concertInfo;
  if (isMenuChanged) {
    rotaryEncoder.setBoundaries(0, 2, true);  // min value, max value(Number of options on menu), loop back(after 2 comes back to 0)
    lcd.clear();
    lcd.setCursor(titlePosition, 0);
    lcd.print("Concert Info");
    lcd.setCursor(2, 1);
    lcd.print("Next Concert:");
    lcd.setCursor(0, 3);
    lcd.print("> EXIT");
    concertInfo = httpGetRequest_Concert();
    lcd.setCursor(2, 2);
    lcd.print(concertInfo.substring(0, 0 + lcdWidth + -2));

    MenuIndex = 2;
    isMenuChanged = false;
  }
  rotaryController(1, 3);
  if (rotaryEncoder.isEncoderButtonClicked()) {
    switch (rotaryEncoder.readEncoder()) {
      case 1:  //Current Concert
      case 2:  //Next Concert
      case 3:  //EXIT
        isMenuChanged = true;
        controlMainMenu();
        break;
    }
  }
  unsigned long currentTime = millis();  // Get the current time
  if (currentTime - lastUpdateConcertTime >= ConcertRequestDelay) {
    concertInfo = httpGetRequest_Concert();
    lcd.setCursor(2, 2);
    lcd.print(concertInfo.substring(0, 0 + lcdWidth + -2));
    lastUpdateConcertTime = currentTime;
  }
  int textLength = concertInfo.length();
  if (currentTime - lastConcertScrollTime >= ScrollDelay) {
    for (int i = 0; i <= textLength - (lcdWidth - 2); i++) {
      lcd.setCursor(2, 2);
      lcd.print(concertInfo.substring(i, i + lcdWidth - 2));
    }
  }
}

float lastTempHum;
void controlTemHum() {
  if (isMenuChanged) {
    rotaryEncoder.setBoundaries(0, 0, true);  // min value, max value(Number of options on menu), loop back(after 2 comes back to 0)
    lcd.clear();
    lcd.setCursor(titlePosition, 0);
    lcd.print("Temp & Hum");
    lcd.setCursor(2, 1);
    lcd.print("Temp.: " + String(sensorDataBuffer[dataBufferIndex].temperature) + " c");
    lcd.setCursor(2, 2);
    lcd.print("Humidity: " + String(sensorDataBuffer[dataBufferIndex].humidity) + "%");
    lcd.setCursor(0, 3);
    lcd.print("> EXIT");
    MenuIndex = 1;
    isMenuChanged = false;
    lastTempHum = sensorDataBuffer[dataBufferIndex].temperature + sensorDataBuffer[dataBufferIndex].humidity;
  }
  rotaryController(3, 3);
  if (rotaryEncoder.isEncoderButtonClicked()) {
    switch (rotaryEncoder.readEncoder()) {
      case 0:  //EXIT
        isMenuChanged = true;
        controlMainMenu();
        break;
    }
  }
  if (sensorDataBuffer[dataBufferIndex].temperature + sensorDataBuffer[dataBufferIndex].humidity != lastTempHum) {
    lastTempHum = sensorDataBuffer[dataBufferIndex].temperature + sensorDataBuffer[dataBufferIndex].humidity;
    lcd.setCursor(9, 1);
    lcd.print(String(sensorDataBuffer[dataBufferIndex].temperature) + " c");
    lcd.setCursor(12, 2);
    lcd.print(String(sensorDataBuffer[dataBufferIndex].humidity) + "%");
  }
}
void controlDisplayConnections() {
}