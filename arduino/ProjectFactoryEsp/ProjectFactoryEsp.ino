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
    //DHT SENSOR
    #define TOKEN_ADDRESS 0 
    #define EEPROM_SIZE 100
    #define DHTPIN 4 
    #define DHTTYPE DHT22
    String serverName = "http://85.246.121.181:8080/"; 
    #define API_KEY "134a8fb6-3824-497a-a23c-1b89abe03a8a"
    #define ROTARY_ENCODER_A_PIN 35
    #define ROTARY_ENCODER_B_PIN 34
    #define ROTARY_ENCODER_BUTTON_PIN 25
    #define ROTARY_ENCODER_STEPS 2
    #define BTN_EMERGENCY 20
    const char* filename = "/data.bin";
    DHT dht(DHTPIN, DHTTYPE);
    int lcdHeight = 4;
    int lcdWidth = 20;
    LiquidCrystal_I2C lcd(0x27,lcdWidth,lcdHeight);
    AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);
    //const char* ssid = "MEO-52EE71";
    //const char* password = "7A41701C96";

    const char* ssid = "esp32wifi";
    const char* password = "espfrancisco";
    void IRAM_ATTR readEncoderISR(){
        rotaryEncoder.readEncoder_ISR();
    }
    void setupRotaryControl(){
      rotaryEncoder.begin();
      rotaryEncoder.setup(readEncoderISR);
      rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
    }
    void setupDHTsensor(){
      dht.begin();
    }
    void setupDisplay(){
      lcd.init();
      lcd.backlight();
    }

    unsigned long lastEpochUpdatedTime = 0;
    unsigned long epochTimerDelay = 10*60*1000; //10 minutes 
    const char* ntpServer = "pool.ntp.org";
    unsigned long oldEpochTime;
    unsigned long epochTime;
    bool firstStart = true;
    unsigned long getTime(){
      while(true){
        unsigned long currentCpuTime = millis();
        if ((currentCpuTime - lastEpochUpdatedTime) > epochTimerDelay || firstStart) {
          firstStart =  false;
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
        unsigned long elapsedCpuTime = (currentCpuTime - lastEpochUpdatedTime)/1000;
        unsigned long currentEpochTime =  oldEpochTime + elapsedCpuTime;
        return currentEpochTime;
      }
    }

    void setupWifi(){
      WiFi.begin(ssid, password);
      lcd.clear();
      lcd.setCursor(0,0);   //Set cursor to character 0 line 1
      lcd.print("Connecting");
      int n = 0;
      while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        lcd.setCursor(n+10,0); 
        lcd.print(".");
        n++;
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Connected WiFi_IP: ");
      lcd.setCursor(0,1);
      lcd.print(WiFi.localIP());
      delay(1000);
    }
    struct SensorData {
      unsigned long timestamp;
      float humidity;
      float temperature;
    };
    unsigned long lastUpdateDataTime = 0; // Variable to store the last time the temperature and humidity were updated
    int dataReadingDelay = 100; // Delay between updates in miliseconds
    int dataBufferIndex = 0;
    const int dataBufferSize = 10;
    SensorData sensorDataBuffer[dataBufferSize];
    void sensorDataController(){ // soon to be universal function to save information
      unsigned long currentTime = millis(); // Get the current time
      if(currentTime - lastUpdateDataTime >= dataReadingDelay){
        lastUpdateDataTime = currentTime;
        sensorDataBuffer[dataBufferIndex].humidity = dht.readHumidity();
        sensorDataBuffer[dataBufferIndex].temperature = dht.readTemperature();
        sensorDataBuffer[dataBufferIndex].timestamp = getTime();
        dataBufferIndex ++;
        if(dataBufferIndex >= dataBufferSize){
          dataBufferIndex = 0;
          if(!SD.begin()){
            Serial.println("SD card not available");
            return;
          }
          writeSensorDataToFile();
        }
      }
    }
    void sendSensorData() {
      File file = SD.open(filename, FILE_READ);
      if (!file) {
        Serial.println("Failed to open file for reading.");
        return;
      }
      size_t filesize = file.size();
      Serial.println("Size: "+String(filesize));
      Serial.println("N of Data Structures: "+String(filesize/12));
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
        char entry[sizeof(SensorData)*10];
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
      if (SD.remove(filename)) {
        Serial.println("File deleted successfully.");
      } else {
        Serial.println("Failed to delete file.");
      }
      
      Serial.println("It Took : "+String((millis()-stopwatch))+" ms");
    }
    void writeSensorDataToFile() {
      static size_t lastSize;
      int numberOfBuffersInSD = 5; 
      size_t maxSDFileSize = sizeof(struct SensorData) * dataBufferSize * numberOfBuffersInSD;
      File file = SD.open(filename, FILE_APPEND);
      if (!file) {
        Serial.println("Failed to open file for writing.");
        return;
      }
      for (int i = 0; i < dataBufferSize; i++) {
        Serial.print(String(file.write((uint8_t*)&sensorDataBuffer[i], sizeof(SensorData)))+" | ");
      }
      Serial.println("");
      size_t fileSize = file.size();
      file.close();
      Serial.println(fileSize);
      if(lastSize == fileSize){
        //SD.remove(filename);
      }
      if(fileSize >= maxSDFileSize){
        sendSensorData();
      }
      lastSize = fileSize;
    }
    void setupSDCard(){
      if (!SD.begin()) {
          Serial.println("Card Mount Failed");
          return;
        }
        uint8_t cardType = SD.cardType();
        if(cardType == CARD_NONE){
          Serial.println("No SD card attached");
          return;
        }
          Serial.print("SD Card Type: ");
        if(cardType == CARD_MMC){
          Serial.println("MMC");
        } else if(cardType == CARD_SD){
          Serial.println("SDSC");
        } else if(cardType == CARD_SDHC){
          Serial.println("SDHC");
        } else {
          Serial.println("UNKNOWN");
      }
      uint64_t cardSize = SD.cardSize() / (1024 * 1024);
      Serial.printf("SD Card Size: %lluMB\n", cardSize);
    }

  //
  //---------------------------AUTHENTHICATION---------------------------------------------------------------------
  //
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
    String serverAPICheckToken() {
      HTTPClient http;
      http.begin((serverName + "api/node/authenticate").c_str());//THIS REQUEST WILL EITHER RETURN THE NODE TOKEN OR NOTHING
      http.addHeader("Content-Type", "application/json");
      http.addHeader("macaddress", WiFi.macAddress());
      http.addHeader("auth_token", readTokenEEPROM());
      http.addHeader("api_key", API_KEY);
      int httpResponseCode = http.GET();
      if (httpResponseCode==200) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println("Payload: "+payload);
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (error) {
          Serial.print("Failed to parse JSON: ");
          Serial.println(error.c_str());
          return "Error" ;
        }
      const char* token = doc["result"]["token"];
      if(String(token)== ""){
        Serial.println("CorrectToken");
      }else{
        saveTokenEEPROM(token);
      }
      Serial.println(String(token));
      return String(token);
      }else{
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
        return "Error";
      }
      http.end();
    }
    void authenticateWithServer(){
      String token = readTokenEEPROM ();
      token = "";
      String response = serverAPICheckToken();
    }

  //
  //-----------------------------SETUP/LOOP------------------------------
  //
    void setup() {
      EEPROM.begin(EEPROM_SIZE);
      Serial.begin(115200);
      setupDisplay();
      setupWifi();
      authenticateWithServer();
      configTime(0, 0, ntpServer);
      setupDHTsensor();
      setupSDCard();
      setupRotaryControl();
      
    }
    void loop() {
      sensorDataController();
      ControlMenu();
    }
    String httpGetRequest_Concert(){
      String payload;
      if(WiFi.status() == WL_CONNECTED){
        HTTPClient http;
        String serverPath = serverName + "api/concerts/get";
        http.begin(serverPath.c_str());
        int httpResponseCode = http.GET();
        if (httpResponseCode>0) {
          payload = http.getString();
        }
        else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
          return "Server Connection err";
        }
        // Free resources
        http.end();
      }else{
        return "NaN";
      } 
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, payload);  
      if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        return "Error";
      }
      const char* band = doc["concert"]["band"];
      const char* time = doc["concert"]["time"];
      return String(band)+ " "+ String(time);
    }
    void sendData(char* payload) {
      HTTPClient http;

      // Specify content-type header
      http.begin(serverName + "api/node/put");
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
        Serial.println("Error during POST request.");
      }

      // Close connection
      http.end();
    }


    //
    //------------------------MENU---------------------------------
    //
    void rotaryController(int minOption, int maxOption){//minOption, on lcd first option line, Max, lcd last option line
      if (rotaryEncoder.encoderChanged()) {
          for(int i = minOption ; i <= maxOption; i++){
            lcd.setCursor(0,i);   //Set cursor to character 0 line 1
            lcd.print(" ");
          }
          lcd.setCursor(0, rotaryEncoder.readEncoder()+minOption);
          lcd.print(">");
      }
    }
    bool isMenuChanged = true;
    int MenuIndex = 0;// 0 = menu / 1 = temphumDisplay / 2 = ConcertDisplay
    int optionIndex = 0;//What option is selected
    int titlePosition = 2;
    void ControlMenu(){
      switch(MenuIndex){
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
    void controlMainMenu(){
      if(isMenuChanged){ //INITIALIZATION
        rotaryEncoder.setBoundaries(0, 2, true);// min value, max value(Number of options on menu), loop back(after 2 comes back to 0) 
        
        lcd.clear();
        lcd.setCursor(titlePosition,0);   //Set cursor to character 2 on line 0
        lcd.print("MENU");
        lcd.setCursor(2,1);   //Set cursor to character 2 on line 0
        lcd.print("Temperatura & Hum");
        lcd.setCursor(0,1);   //Set cursor to character 2 on line 0
        lcd.print(">");
        lcd.setCursor(2,2);   //Set cursor to character 2 on line 0
        lcd.print("Concertos");
        lcd.setCursor(2,3);   //Set cursor to character 2 on line 0
        lcd.print("Connections");
        MenuIndex = 0;
        isMenuChanged = false;
      }
      rotaryController(1,3);
      //Option Controller
      if (rotaryEncoder.isEncoderButtonClicked()) {
          switch(rotaryEncoder.readEncoder()){
            case 0://Temperatura e humidade
              isMenuChanged = true;
              controlTemHum();
              break;
            case 1://Concertos
              isMenuChanged = true;
              controlDisplayConcert();
              break;
            case 2://Connections
              isMenuChanged = true;
              controlDisplayConnections();
              Serial.println("Connection");  
              break;
          }
      }
    }
    unsigned long lastUpdateConcertTime = 0;
    unsigned long lastConcertScrollTime = 0;
    void controlDisplayConcert(){
      int ConcertRequestDelay = 30000; //30 seconds
      int ScrollDelay=500;
      String concertInfo;
      if(isMenuChanged){
        rotaryEncoder.setBoundaries(0, 2, true);// min value, max value(Number of options on menu), loop back(after 2 comes back to 0) 
        concertInfo = httpGetRequest_Concert();
        lcd.clear();
        lcd.setCursor(titlePosition,0); 
        lcd.print("Concert Info");
        lcd.setCursor(2,1);
        lcd.print("Next Concert:");
        lcd.setCursor(2,2);
        lcd.print(concertInfo.substring(0, 0 + lcdWidth+-2));
        lcd.setCursor(0,3); 
        lcd.print("> EXIT");
        MenuIndex = 2;
        isMenuChanged = false;
      }
      rotaryController(1,3);
      if (rotaryEncoder.isEncoderButtonClicked()) {
        switch(rotaryEncoder.readEncoder()){
          case 1://Current Concert
          case 2: //Next Concert
          case 3://EXIT
            isMenuChanged = true;
            controlMainMenu();
            break;
        }
      }
      unsigned long currentTime = millis(); // Get the current time
      if(currentTime - lastUpdateConcertTime >= ConcertRequestDelay){
        concertInfo = httpGetRequest_Concert();
        lcd.setCursor(2,2);
        lcd.print(concertInfo.substring(0, 0 + lcdWidth+-2));
        lastUpdateConcertTime = currentTime;
      }
      int textLength = concertInfo.length();
      if(currentTime - lastConcertScrollTime >= ScrollDelay){
        for (int i = 0; i <= textLength - (lcdWidth-2); i++) {
            lcd.setCursor(2,2);
            lcd.print(concertInfo.substring(i, i + lcdWidth-2));
        }
      }
    }

    float lastTempHum;
    void controlTemHum(){
          if(isMenuChanged){
            rotaryEncoder.setBoundaries(0, 0, true);// min value, max value(Number of options on menu), loop back(after 2 comes back to 0) 
            lcd.clear();
            lcd.setCursor(titlePosition,0); 
            lcd.print("Temp & Hum");
            lcd.setCursor(2,1);
            lcd.print("Temp.: "+String(sensorDataBuffer[dataBufferIndex].temperature)+" c");
            lcd.setCursor(2,2);
            lcd.print("Humidity: "+String(sensorDataBuffer[dataBufferIndex].humidity)+"%");
            lcd.setCursor(0,3); 
            lcd.print("> EXIT");
            MenuIndex = 1;
            isMenuChanged = false;
            lastTempHum = sensorDataBuffer[dataBufferIndex].temperature+sensorDataBuffer[dataBufferIndex].humidity;
          }
          rotaryController(3,3);
          if (rotaryEncoder.isEncoderButtonClicked()) {
            switch(rotaryEncoder.readEncoder()){
              case 0://EXIT
                isMenuChanged = true;
                controlMainMenu();
                break;
            }
          }
          if(sensorDataBuffer[dataBufferIndex].temperature+sensorDataBuffer[dataBufferIndex].humidity != lastTempHum){
            lastTempHum = sensorDataBuffer[dataBufferIndex].temperature+sensorDataBuffer[dataBufferIndex].humidity;
            lcd.setCursor(9,1);
            lcd.print(String(sensorDataBuffer[dataBufferIndex].temperature)+" c");
            lcd.setCursor(12,2);
            lcd.print(String(sensorDataBuffer[dataBufferIndex].humidity)+"%");
          }
    }
    void controlDisplayConnections(){
    }