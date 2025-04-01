#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "aws_certificates.h"
#include <time.h>

// WiFi Credentials
const char* ssid = "lol";
const char* password = "dsce@123";

// AWS IoT Core Config
const char* aws_endpoint = "a223f6z9oxujhi-ats.iot.ap-south-1.amazonaws.com";
const char* mqtt_topic = "air_qualityproject";

// Sensor Configuration
#define MQ135_PIN 34  // ADC pin for MQ135

// Clients
WiFiClientSecure net;
PubSubClient client(net);

// Function: Sync time with NTP
void syncTime() {
  configTime(19800, 0, "pool.ntp.org", "time.nist.gov");  // UTC+5:30
  Serial.print("Syncing time");
  while (time(nullptr) < 100000) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nTime synced!");
}

// Function: Get current timestamp
String getTimestamp() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "unknown";
  char buf[30];
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &timeinfo);
  return String(buf);
}

// Function: Connect to WiFi
void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nWiFi connected");
  syncTime();
}

// Function: Connect to AWS IoT Core
void connectAWS() {
  net.setCACert(AWS_ROOT_CA);
  net.setCertificate(AWS_DEVICE_CERT);
  net.setPrivateKey(AWS_PRIVATE_KEY);

  client.setServer(aws_endpoint, 8883);
  client.setProtocol(MQTT_VERSION_3_1_1); // IMPORTANT FIX

  while (!client.connected()) {
    Serial.print("Connecting to AWS IoT...");
    if (client.connect("ESP32_AirQualityClient")) {
      Serial.println("Connected to AWS IoT Core!");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

// Function: Read sensor and publish data
void publishAirQuality() {
  int rawValue = analogRead(MQ135_PIN);
  float voltage = rawValue * (3.3 / 4095.0);

  StaticJsonDocument<256> doc;
  doc["sensor"] = "MQ135";
  doc["raw"] = rawValue;
  doc["voltage"] = voltage;
  doc["timestamp"] = getTimestamp();

  char payload[256];
  serializeJson(doc, payload);

  if (client.publish(mqtt_topic, payload)) {
    Serial.println("Published to AWS IoT:");
    Serial.println(payload);
  } else {
    Serial.println(" Failed to publish data");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  connectWiFi();
  connectAWS();
}

void loop() {
  if (!client.connected()) {
    connectAWS();
  }
  publishAirQuality();
  client.loop();
  delay(10000);  // Publish every 10 seconds
}
