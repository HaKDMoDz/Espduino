#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <DHT.h>
#include "DHT_Async.h"
#include <ArduinoOTA.h>
#include <deque>
//#include <ESP8266mDNS.h>

#define DHT_SENSOR_TYPE DHT_TYPE_11
const char* ssid = "NETGEAR80";
const char* password = "dailycoconut482";
const int DHT_SENSOR_PIN = D4 ;
const int light1 = D1;
const int light2 = D2;
const int light3 = D5;
const int heat = D6;
const int fan = D7;

bool measureEnvironment(float *temperature, float *humidity);
bool light1State = false;
bool light2State = false;
bool light3State = false;
bool fanState = false;
bool heatState = false;


void handleRoot();
void handleDiagram();
void handleLight1();
void handleLight2();
void handleLight3();
void handleFan();
void handleHeat();
void handleRemoteSerial();
void setupOTA();
void handleSerialOutput();

void serveFile(const char* filename, const char* contentType);


std::deque<String> serialBuffer;
const int BUFFER_SIZE = 10;

DHT_Async dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);
ESP8266WebServer server(80);

// Set your Static IP address
IPAddress local_IP(192, 168, 0, 50);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

void setup() {
  pinMode(light1, OUTPUT);
  pinMode(light2, OUTPUT);
  pinMode(light3, OUTPUT);
  pinMode(heat, OUTPUT);
  pinMode(fan, OUTPUT);
  digitalWrite(light1, LOW);
  digitalWrite(light2, LOW);
  digitalWrite(light3, LOW);
  digitalWrite(heat, LOW);
  digitalWrite(fan, LOW);
  Serial.println("DHT_SENSOR");
  //  Serial.println(L1);
  //  Serial.println(L2);
  //  Serial.println(L3);
  //  Serial.println(H);
  //  Serial.println(F);

  server.on("/serial-output", HTTP_GET, handleSerialOutput); // New endpoint for serial output
  Serial.begin(115200);
  Serial.println();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  
  Serial.println();
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }
  Serial.println();
  Serial.println("Server Endpoints");
  server.on("/", handleRoot);
  Serial.println("/ root");
  //  server.on("/pins", handlePinsPageRequest);
  //    Serial.println("/pins");
  server.on("/light1", handleLight1);
  Serial.println("/light1");
  server.on("/light2", handleLight2);
  Serial.println("/light2");
  server.on("/light3", handleLight3);
  Serial.println("/light3");
  server.on("/fan", handleFan);
  Serial.println("/fan");
  server.on("/heat", handleHeat);
  Serial.println("/heat");
  server.on("/serial", HTTP_POST, handleRemoteSerial);
  Serial.println("/serial");
  // Serve files
  server.on("/3d.jpg", HTTP_GET, []() {
    serveFile("/3d.jpg", "image/jpeg");
  });
  server.on("/banner.png", HTTP_GET, []() {
    serveFile("/banner.png", "image/png");
  });
  server.on("/Esp8266.png", HTTP_GET, []() {
    serveFile("/Esp8266.png", "image/png");
  });
  server.on("/off2.jpg", HTTP_GET, []() {
    serveFile("/off2.jpg", "image/jpeg");
  });
  server.on("/on2.jpg", HTTP_GET, []() {
    serveFile("/on2.jpg", "image/jpeg");
  });
  server.on("/switchSound.wav", HTTP_GET, []() {
    serveFile("/switchSound.wav", "audio/wav");
  });
  server.on("/vanduinopins.png", HTTP_GET, []() {
    serveFile("/vanduinopins.png", "image/png");
  });
  server.begin();
  Serial.println("HTTP server started");
  setupOTA();
}
void loop() {
  server.handleClient();
  ArduinoOTA.handle();
  //MDNS.update();
  //% TEMPERATURE % = dht_sensor.measure(temperature);
  //  % HUMIDITY %= dht_sensor.measure(humidity);
}
void handleRemoteSerial() {
  if (server.hasArg("message")) {
    String message = server.arg("message");
    Serial.println("Received from remote: " + message);
    serialBuffer.push_back("Received: " + message);
    if (serialBuffer.size() > BUFFER_SIZE) {
      serialBuffer.pop_front(); // Remove oldest message if buffer exceeds size
    }
    server.send(200, "application/json", "{\"status\": \"success\", \"message\": \"Message received\"}");
  } else {
    server.send(400, "application/json", "{\"status\": \"error\", \"message\": \"No message provided\"}");
  }
}
void handleSerialOutput() {
  String output = "[";
  for (size_t i = 0; i < serialBuffer.size(); i++) {
    output += "\"" + serialBuffer[i] + "\"";
    if (i < serialBuffer.size() - 1) {
      output += ",";
    }
  }
  output += "]";
  server.send(200, "application/json", output);
  if (server.hasArg("message")) {
    String message = server.arg("message");
    Serial.println("Received from remote: " + message);
    server.send(200, "application/json", "{\"status\": \"success\", \"message\": \"Message received\"}");
  } else {
    server.send(400, "application/json", "{\"status\": \"error\", \"message\": \"No message provided\"}");
  }
}
void handleRoot() {
  serveFile("/index.html", "text/html");
}
//void handlePinsPageRequest() {
//  serveFile("/pins.html", "text/html");
//}
void handleLight1() {
  light1State = !light1State; // Toggle the state
  digitalWrite(light1, light1State ? HIGH : LOW);
  String response = String("{\"status\": \"success\", \"message\": \"Light 1 ") + (light1State ? "on" : "off") + "\"}";
  server.send(200, "application/json", response);
  Serial.println("Light1 Handled Successfully");
}
void handleLight2() {
  light2State = !light2State; // Toggle the state
  digitalWrite(light2, light2State ? HIGH : LOW);
  String response = String("{\"status\": \"success\", \"message\": \"Light 2 ") + (light2State ? "on" : "off") + "\"}";
  server.send(200, "application/json", response);
  Serial.println("Light2 Handled Successfully");
}
void handleLight3() {
  light3State = !light3State; // Toggle the state
  digitalWrite(light3, light3State ? HIGH : LOW);
  String response = String("{\"status\": \"success\", \"message\": \"Light 3 ") + (light3State ? "on" : "off") + "\"}";
  server.send(200, "application/json", response);
  Serial.println("Light3 Handled Successfully");
}
void handleFan() {
  fanState = !fanState; // Toggle the state
  digitalWrite(fan, fanState ? HIGH : LOW);
  String response = String("{\"status\": \"success\", \"message\": \"Fan ") + (fanState ? "on" : "off") + "\"}";
  server.send(200, "application/json", response);
  Serial.println("Fan Handled Successfully");
}
void handleHeat() {
  heatState = !heatState; // Toggle the state
  digitalWrite(heat, heatState ? HIGH : LOW);
  String response = String("{\"status\": \"success\", \"message\": \"Heater ") + (heatState ? "on" : "off") + "\"}";
  server.send(200, "application/json", response);
  Serial.println("Heat Handled Successfully");
}
void serveFile(const char* filename, const char* contentType) {
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    server.send(404, "text/plain", "File Not Found");
    return;
  }
  server.streamFile(file, contentType);
  file.close();
}
bool measureEnvironment(float *temperature, float *humidity) {
  static unsigned long measurement_timestamp = millis();
  if (millis() - measurement_timestamp > 4000ul) {
    if (dht_sensor.measure(temperature, humidity)) {
      measurement_timestamp = millis();
      return true;
    }
  }
  return false;
}
void setupOTA() {
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch"; // firmware
    } else { // U_SPIFFS
      type = "filesystem"; // SPIFFS
    }
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}
