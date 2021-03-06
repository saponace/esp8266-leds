#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Adafruit_NeoPixel.h>
#include <SingleColor.h>


const char* SSID = "SFR-b118";
const char* PASSWORD = "V6AW5I2E1S2M";
const int PORT = 80;
const int LED_STRIP_NB_LEDS = 300;
const int LED_STRIP_DATA_PIN = D1;
const int NETWORK_LED_PIN = D2;
const int SERVER_ON_LED_PIN = D3;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_STRIP_NB_LEDS, LED_STRIP_DATA_PIN, NEO_GRB + NEO_KHZ800);

ESP8266WebServer server(PORT);

void blinkNetworkLed() {
  int ledDuration = 100;
  digitalWrite(NETWORK_LED_PIN, 1);
  delay(ledDuration);
  digitalWrite(NETWORK_LED_PIN, 0);
}

void handleRoot() {
  blinkNetworkLed();
  server.send(200, "text/plain", "hello from esp8266!");
}

int getArgValue(String name){
  for (uint8_t i = 0; i < server.args(); i++)
  if(server.argName(i) == name)
  return server.arg(i).toInt();
  return -1;
}



void setColor(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

void handleOneColor() {
  blinkNetworkLed();
  SingleColor singleColor (server);
  if(singleColor.checkColorsValuesInRange().hasError()){
    server.send(400, "text/plain", singleColor.checkColorsValuesInRange().getErrorMsg());
  }
  else {
    String message = singleColor.toString();
    server.send(200, "text/plain", message);
    // leds.send(strip);
    setColor(strip.Color(singleColor.getRed(), singleColor.getGreen(), singleColor.getBlue()));
  }
}

void handleNotFound() {
  blinkNetworkLed();
  String message = "Color\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


void defineRoutes(){
  server.on("/", handleRoot);
  server.on("/color", handleOneColor);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
}

void startServer(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\n");

  server.begin();

  Serial.println("HTTP server started");
  Serial.print("Connected to ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(SERVER_ON_LED_PIN, 1);
}

void setup(void) {
  pinMode(LED_STRIP_DATA_PIN, OUTPUT);
  pinMode(NETWORK_LED_PIN, OUTPUT);
  pinMode(SERVER_ON_LED_PIN, OUTPUT);
  digitalWrite(NETWORK_LED_PIN, 0);
  digitalWrite(SERVER_ON_LED_PIN, 0);
  Serial.begin(115200);

  strip.begin();
  strip.show();

  startServer();
  defineRoutes();
}

void loop(void) {
  server.handleClient();
}
