/* 
 Share SIM7000G code
 change lines between rows of ##############
*/

//lilygo sim7000G configure
  #define SerialMon Serial // Set serial for debug console (to the Serial Monitor, default speed 115200)
  #define SerialAT Serial1 // Set serial for AT commands (to the module)
  #define TINY_GSM_DEBUG SerialMon  // #define DUMP_AT_COMMANDS  // See all AT commands, if wanted
  #define TINY_GSM_MODEM_SIM7000
  #define TINY_GSM_RX_BUFFER 1024   // Set RX buffer to 1Kb
  #define GSM_AUTOBAUD_MIN 9600     // Range to attempt to autobaud
  #define GSM_AUTOBAUD_MAX 115200   // Range to attempt to autobaud
  #define TINY_GSM_USE_GPRS true
  #define TINY_GSM_USE_WIFI false
  #define TINY_GSM_TEST_GPRS    true
  #define TINY_GSM_TEST_GPS     true
  #define TINY_GSM_POWERDOWN    true
  #define GSM_PIN "" // set GSM PIN, if any
//BME library
  #include <Adafruit_BME280.h>
  Adafruit_BME280 bme; // use I2C interface
//GPRS credentials
//############################### CHANGE THINGS BETWEEN THIS SPACE ################################################################
 const char apn[]      = "INSERT YOUR APN HERE";   //  APN
  const char gprsUser[] = "insert GPRS username here";  //  GPRS User
  const char gprsPass[] = "insert GPRS passwrd here";  //  GPRS Password
//Server details
  const char server[] = "insert server address here";      //set the rest of the api call below
  const char resource[] = "insert your resource address here";
//API Key and Port Number
  String apiKeyValue  = "API Key Here"; // this can be changed to anything but must be same in file
  const int  port = YOUR_PORT_NUMBER;
//#################################################################################################################################

  #include <TinyGsmClient.h>
 // #include <ArduinoHttpClient.h>
  #include <SPI.h>
  #include <SD.h>
  #include <Ticker.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>

  #define SCREEN_WIDTH 128 // OLED display width, in pixels
  #define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

  #ifdef DUMP_AT_COMMANDS  // if enabled it requires the streamDebugger lib
    #include <StreamDebugger.h>
    StreamDebugger debugger(SerialAT, Serial);
    TinyGsm modem(debugger);
  #else
    TinyGsm modem(SerialAT);
  #endif
  TinyGsmClient client(modem);
//  HttpClient http(client, server, port);
//gpio pins
  #define PIN_TX      27
  #define PIN_RX      26
  #define PWR_PIN     4
  int mPower=25;

//variables
  #define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
  String phpDate, comments, fName, dName, tName;
  float bmeT, bmeH, bmeD, bmeP;
  long secondsLeft, startTime, timeSoFar;
  long secsToSleep =  5 /*minutes*/ *60 ;  /* Time to sleep (minutes) */
 
void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialise the OLED Display
  display.clearDisplay();                    // Clear the OLED Buffer
  
  startTime=millis();
 // Serial.begin(115200); Serial.println(__FILE__);
//set modem power pin
  pinMode(mPower,OUTPUT); digitalWrite(25,HIGH);
//initialise bme280 
  bme.begin(0x76);
//Set modem power
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, HIGH);
  delay(300);
  digitalWrite(PWR_PIN, LOW);

  //Serial.println("\nWait...");
  SerialAT.begin(115200, SERIAL_8N1, PIN_RX, PIN_TX);
// --------------------------OLED BEGIN----------------------------------------
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(25, 8);
  display.println("\nWait...");     // Display static text
  display.display();
// ---------------------------OLED END-----------------------------------------
    delay(6000);
  modem.simUnlock(GSM_PIN);
 // Serial.println("---RESETTING SIM");
// --------------------------OLED BEGIN----------------------------------------
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(25, 8);
  display.println("-RESETTING SIM-");     // Display static text
  display.display();
// ---------------------------OLED END----------------------------------------- 
    modem.sendAT("+CFUN=1,1");
  if (!modem.init()) {
      modem.restart();
      delay(2000);
     // Serial.println("Failed to restart modem, attempting to continue without restarting");
// --------------------------OLED BEGIN----------------------------------------
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(25, 8);
  display.println("Failed to");     // Display static text
  display.println("restart modem,");
  display.println("atmptng to cotinu");
  display.println("without restarting");
  display.display();
// ---------------------------OLED END----------------------------------------- 
      // return;
  }

  String name = modem.getModemName();
    delay(500);
  //Serial.println("Modem Name: " + name);
  // --------------------------OLED BEGIN----------------------------------------
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(28, 0);
  display.println("Modem Name: ");     // Display static text
  display.setCursor(20, 9);
  display.println(name);
  display.display();
// ---------------------------OLED END-----------------------------------------
  String modemInfo = modem.getModemInfo();
    delay(500);
 // Serial.println("Modem Info: " + modemInfo);
  // --------------------------OLED BEGIN----------------------------------------
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(28, 0);
  display.println("Modem Info: ");     // Display static text
  display.setCursor(0, 9);
  display.println(modemInfo);
  display.display();
// ---------------------------OLED END-----------------------------------------

  // Set SIM7000G GPIO4 LOW ,turn off GPS power
  // CMD:AT+SGPIO=0,4,1,0
  // Only in version 20200415 is there a function to control GPS power
  modem.sendAT("+SGPIO=0,4,1,0");

  #if TINY_GSM_USE_GPRS
    // Unlock your SIM card with a PIN if needed
    if ( GSM_PIN && modem.getSimStatus() != 3 ) {
      modem.simUnlock(GSM_PIN);
    }
  #endif

//set network mode - 2=Automatic, 51=GSM and LTE only, 13=GSM only, 38=LTE only
   String res;
   res = modem.setNetworkMode(2); 
   delay(500);
   //SerialMon.println(res);
   // --------------------------OLED BEGIN----------------------------------------
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(28, 0);
  // Display static text
  display.println(res);
  display.display();
// ---------------------------OLED END-----------------------------------------

 // Configure the wake up source as timer wake up  
  //esp_sleep_enable_timer_wakeup(secsToSleep * uS_TO_S_FACTOR);

}

void loop() {
//define comments
    fName=__FILE__; fName=fName.substring(fName.lastIndexOf("\\")+1,fName.length());
    dName= __DATE__; tName=__TIME__;
    comments=fName+" "+dName+"-"+tName;
  //get bme details 
    bmeH=bme.readHumidity();
    bmeT=bme.readTemperature();
    bmeD=243.04*(log(bmeH/100)+(17.625*bmeT/(243.04+bmeT)))/(17.625-log(bmeH/100)-(17.625*bmeT/(243.04+bmeT)));
    bmeP=bme.readPressure()/100.0F;
    //Serial.print(__LINE__); Serial.print("> ");Serial.print(bmeT);Serial.print("\t");Serial.print(bmeH);Serial.print("\t");Serial.print(bmeD);Serial.print("\t");Serial.println(bmeP);
  
//send details to mysql
  apiCall();
  
//turn off modem
  digitalWrite(mPower,LOW);

//power off modem and put ESP32 into deep sleep mode (with timer wake up)
  modem.poweroff();
//Configure ttime left before wake up  
  //Serial.println("sleep time: " + String(secsToSleep));
  long sTemp=millis();
  timeSoFar = sTemp-startTime;
  timeSoFar = timeSoFar/1000;
    //Serial.println("seconds so far: " + String(timeSoFar));
  secondsLeft=secsToSleep-timeSoFar;
    //Serial.println("seconds to sleep: " + String(secondsLeft));
  esp_sleep_enable_timer_wakeup(secondsLeft * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void apiCall(){
    //SerialMon.print("Waiting for network...");
    // --------------------------OLED BEGIN----------------------------------------
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(28, 0);
  // Display static text
  display.println("Waiting");
  display.setCursor(0, 16);
  display.println("for network...");
  display.display();
// ---------------------------OLED END-----------------------------------------
    if (!modem.waitForNetwork()) {
     // SerialMon.println(" fail");
   // --------------------------OLED BEGIN----------------------------------------
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(14, 0);
  display.println("Network");
  display.setCursor(28, 24);
  display.println("Fail!");   // Display static text
  display.display();
// ---------------------------OLED END-----------------------------------------     
      delay(5000);
      return;
    }
    //SerialMon.println(" success");
  // --------------------------OLED BEGIN----------------------------------------
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(14, 0);
  display.println("Network");
  display.setCursor(14, 24);
  display.println("Success");       // Display static text
  display.display();
// ---------------------------OLED END-----------------------------------------

    if (modem.isNetworkConnected()) {
      //SerialMon.println("Network connected");
  // --------------------------OLED BEGIN----------------------------------------
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(14, 0);
  // Display static text
  display.println("Network");
   display.setCursor(14, 24);
  display.println("Connected");
  display.display();
// ---------------------------OLED END-----------------------------------------
    }

      //SerialMon.print(F("Connecting to "));
      //SerialMon.print(apn);
// --------------------------OLED BEGIN----------------------------------------
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(28, 0);
  // Display static text
  display.println(F("Connecting to "));
  display.setCursor(0, 16);
  display.println(apn);
  display.display();
// ---------------------------OLED END-----------------------------------------
      if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        //SerialMon.println(" fail");
// --------------------------OLED BEGIN----------------------------------------
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(28, 0);
  display.println("GPRS");
  display.setCursor(28, 24);
  display.println("Fail!");   // Display static tex
  display.display();
// ---------------------------OLED END-----------------------------------------
        delay(10000);
        return;
      }
      //SerialMon.println(" success");
// --------------------------OLED BEGIN----------------------------------------
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(28, 0);
  display.println("GPRS");
  display.setCursor(28, 24);
  display.println("Success");   // Display static tex
  display.display();
// ---------------------------OLED END-----------------------------------------

      if (modem.isGprsConnected()) {
        //SerialMon.println("GPRS connected");
// --------------------------OLED BEGIN----------------------------------------
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(28, 0);
  display.println("GPRS");
  display.setCursor(14, 24);
  display.println("CONNECTED");   // Display static tex
  display.display();
// ---------------------------OLED END-----------------------------------------
      }
      modem.sendAT("+SGPIO=0,4,1,1");
      modem.enableGPS();
      float lat,  lon;
     
      modem.disableGPS();
      modem.sendAT("+SGPIO=0,4,1,0");
      //Serial.println("\n---End of GPRS TEST---\n");
// --------------------------OLED BEGIN----------------------------------------
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(28, 0);
  // Display static text
  display.println("\n--End of GPRS ");
  display.println("TEST--\n");
  display.display();
// ---------------------------OLED END-----------------------------------------
//send data to mysql
   //SerialMon.println(F("Performing HTTP POST request... "));
// --------------------------OLED BEGIN----------------------------------------
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(20, 0);
  // Display static text
  display.println("SENDING");
  display.setCursor(23, 24);
  display.println("-DATA-");
  display.display();
// ---------------------------OLED END-----------------------------------------

//SerialMon.print("Connecting to ");
    //SerialMon.print(server);
    if (!client.connect(server, port)) {
      //SerialMon.println(" fail");
// --------------------------OLED BEGIN----------------------------------------
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(25, 0);
  // Display static text
  display.println("SENDING DATA");
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.println("--FAILED--");
  display.display();
// ---------------------------OLED END-----------------------------------------
    }
    else {
      //SerialMon.println(" OK");
    
      // Making an HTTP POST request
      //SerialMon.println("Performing HTTP POST request...");
 // Prepare your HTTP POST request data (Temperature in Celsius degrees)
      String httpRequestData = "api_key=" + apiKeyValue + "&temperature=" + String(bmeT)
                             + "&humidity=" + String(bmeH) + "&dewpoint=" + String(bmeD) + "&pressure=" + String(bmeP) + "";
      // Prepare your HTTP POST request data (Temperature in Fahrenheit degrees)
      //String httpRequestData = "api_key=" + apiKeyValue + "&value1=" + String(1.8 * bme.readTemperature() + 32)
      //                       + "&value2=" + String(bme.readHumidity()) + "&value3=" + String(bme.readPressure()/100.0F) + "";
          
      // You can comment the httpRequestData variable above
      // then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
      //String httpRequestData = "api_key";
    
      client.print(String("POST ") + resource + " HTTP/1.1\r\n");
      client.print(String("Host: ") + server + "\r\n");
      client.println("Connection: close");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.println(httpRequestData.length());
      client.println();
      client.println(httpRequestData);

      unsigned long timeout = millis();
      while (client.connected() && millis() - timeout < 10000L) {
        // Print available data (HTTP response from server)
        while (client.available()) {
          char c = client.read();
          //SerialMon.print(c);
          timeout = millis();
        }
      }
      //SerialMon.println();
    
      // Close client and disconnect
      client.stop();
      //SerialMon.println(F("Server disconnected"));
      modem.gprsDisconnect();
      //SerialMon.println(F("GPRS disconnected"));
      // --------------------------OLED BEGIN----------------------------------------
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  // Display static text
  display.println("SERVER Disconcted");
  display.setCursor(5, 8);
  display.println("GPRS Disconcted");
  display.setCursor(15, 28);
  display.print(bmeT);
  display.print(" C | ");
  display.print(bmeH);
  display.print(" %");
  display.display();
// ---------------------------OLED END-----------------------------------------
    }
  }
 
