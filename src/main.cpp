// ESP8266 WiFi <-> UART Bridge
// by RoboRemo
// www.roboremo.com

// Disclaimer: Don't use RoboRemo for life support systems
// or any other situations where system failure may affect
// user or environmental safety.
//green tx
//yellow rx


#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiClient.h>
//#include <WiFiServer.h>
// config: ////////////////////////////////////////////////////////////

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define UART_BAUD 921600 //aquarios
#define packTimeout 5 // ms (if nothing more on UART, then send packet)
#define bufferSize 8192
#define PROTOCOL_TCP

uint8_t buf1[bufferSize];
uint16_t i1=0;

uint8_t buf2[bufferSize];
uint16_t i2=0;

const char* ssid     = "MikroBridge";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "DarkDay1999";     // The password of the Wi-Fi network
const int port = 9876;
WiFiServer server(port);
WiFiClient client;

void setup() {
  WiFi.begin(ssid, password);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Serial.begin(UART_BAUD);
  delay(10);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.println("Initialization...");
  int i = 0;
  delay(500);
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    display.clearDisplay();
    display.setCursor(0, 10);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.println(++i);
    }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.println("Connected");
  display.setCursor(0, 10);
  display.println(WiFi.localIP());
  display.setCursor(84, 10);
  display.println(":");
  display.setCursor(88, 10);
  display.println(port);
  delay(1000);
  server.begin();
  display.display();
}


void loop() {
  if(!client.connected()) { // if client not connected
    client = server.available(); // wait for it to connect
    return;
  }
  
  // here we have a connected client

  if(client.available()) {
    while(client.available()) {
      buf1[i1] = (uint8_t)client.read(); // read char from client (RoboRemo app)
      if(i1<bufferSize-1) i1++;
    }
    // now send to UART:
    Serial.write(buf1, i1);
    i1 = 0;
  }

  if(Serial.available()) {

    // read the data until pause:
    
    while(1) {
      if(Serial.available()) {
        buf2[i2] = (char)Serial.read(); // read char from UART
        if(i2<bufferSize-1) i2++;
      } else {
        //delayMicroseconds(packTimeoutMicros);
        delay(packTimeout);
        if(!Serial.available()) {
          break;
        }
      }
    }
    
    // now send to WiFi:
    client.write((char*)buf2, i2);
    i2 = 0;
  }
}