#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// assets.h contains our WiFi animation images.
#include "assets.h"

// Setup Attached OLED.
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// Built in LED to play with.
#define LED_BUILTIN  2
static const int ledPin = LED_BUILTIN;

// Initial LED state.
int ledState = LOW;

// WiFi Credentials go here.
static const char* ssid = "*********";
static const char* password = "**********";

// Instansiate Web Server
AsyncWebServer server(80);

// Instansiate display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Provides our Web Page and the state of our LED.
String getHTML(int ledState) {
  String html = "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";
  html += "<title>ESP32 Controller</title>";
  html += "<style>";
  html += "body { text-align: center; font-family: Arial, sans-serif; }";
  html += ".buttonoff { background-color: slateblue; color: white; width: 50px; height: 30px; margin: 2px; border-radius: 10px }";
  html += ".buttonon { background-color: red; color: white; width: 50px; height: 30px; margin: 2px; border-radius: 10px }";
  html += ".buttonoff:hover { background-color: darkslateblue; }";
  html += ".buttonoff:active { background-color: mediumslateblue; }";
  html += "</style>";
  html += "</head>";
  html += "<body>";
  html += "<h1>ESP32 Controller</h1>";
  html += "<p>On Board LED:";
  if (ledState == LOW)
    html += "<a href=\"/led/on\"><button class=buttonoff>ON</button></a>";
  else
    html += "<a href=\"/led/on\"><button class=buttonon>ON</button></a>";
  html += "<a href=\"/led/off\"><button class=buttonoff>OFF</button></a>";
  html += "</p>";
  html += "</body>";
  html += "</html>";

  return html;
}

void setup() {
  // put your setup code here, to run once:
  // Initialize Serial port and pause for 1 second.
  Serial.begin(9600);
  delay(1000);
  Serial.println(); // Pop a quick line, clear junk, fresh for next line.
  Serial.println("Initializing...");

  // Setup LED state.
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  // Initialize display
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear buffer - Fresh Start
  display.clearDisplay();
  display.setTextSize(1);     
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(25, 0);
  display.print("Connecting...");
  display.display();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");

    // WiFi connection images (from assets.h)
    int wifiImages = (sizeof(wifiImgArray) / sizeof(wifiImgArray[0])) - 1;

    // Loop through and display connecting images
    for (int c = 0; c <= wifiImages; c++ ) {
      if (c == wifiImages) {
        display.drawBitmap(39, 17, wifiImgArray[c], 50, 50, 0);
        display.display();
        delay(500);
      }
      else {
        display.drawBitmap(39, 17, wifiImgArray[c], 50, 50, 1);
        display.display();
        delay(500);
      }
    }
  }

  Serial.println("Connected to WiFi");
  // Print the IP address
  Serial.print("ESP32 Web Server's IP address: ");
  Serial.println(WiFi.localIP());

  // We are initialized and ready to go.
  Serial.println("Initialized.");

  // Clean up and let em know we are ready
  // and where to go
  display.clearDisplay();
  display.setTextSize(1);     
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(0, 0);
  display.println("Connected:");
  display.print("http://");
  display.print(WiFi.localIP());
  display.display();

  // home page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("ESP32 Web Server: New request received:");
    Serial.println("GET /");
    request->send(200, "text/html", getHTML(ledState));
  });

  // Route to control the LED for On.
  server.on("/led/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("ESP32 Web Server: New request received:");
    Serial.println("GET /led/on");
    ledState = HIGH;
    digitalWrite(ledPin, ledState);
    request->send(200, "text/html", getHTML(ledState));

    // Display the state of the LED, first make sure
    // The display is blanked from what could displayed 
    // before.
    display.setTextSize(2);     
    display.setTextColor(SSD1306_BLACK); 
    display.setCursor(0, 30);
    display.print("LED Off!");
    display.display();

    // Then display what we want to show.
    display.setTextColor(SSD1306_WHITE); 
    display.setCursor(0, 30);
    display.print("LED On!");
    display.display();
  });
  // Route to control the LED for Off.
  server.on("/led/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("ESP32 Web Server: New request received:");
    Serial.println("GET /led/off");
    ledState = LOW;
    digitalWrite(ledPin, ledState);
    request->send(200, "text/html", getHTML(ledState));

    // Display the state of the LED, first make sure
    // The display is blanked from what could displayed 
    // before.
    display.setTextSize(2);     
    display.setTextColor(SSD1306_BLACK); 
    display.setCursor(0, 30);
    display.print("LED On!");
    display.display();

    // Then display what we want to show.
    display.setTextColor(SSD1306_WHITE); 
    display.setCursor(0, 30);
    display.print("LED Off!");
    display.display();
  });

  // Start the web server
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
}
