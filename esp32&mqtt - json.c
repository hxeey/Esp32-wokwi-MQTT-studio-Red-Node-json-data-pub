#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// WiFi setup
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// MQTT broker
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* clientId = "ruqaya-env-esp32";

// Topic
const char* topic = "ruqaya/sensors/env";

// Pins
#define LDR_PIN 34
#define POT_PIN 33
#define DHT_PIN 15
#define DHT_TYPE DHT22

// Clients
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHT_PIN, DHT_TYPE);

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    client.connect(clientId);
    delay(500);
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int light = analogRead(LDR_PIN);
  int potValue = analogRead(POT_PIN);

  // JSON payload
  StaticJsonDocument<200> doc;
  doc["temp"] = isnan(temperature) ? -1 : temperature;
  doc["humidity"] = isnan(humidity) ? -1 : humidity;
  doc["light"] = light;
  doc["pot"] = potValue;

  char jsonBuffer[256];
  size_t n = serializeJson(doc, jsonBuffer);
  client.publish(topic, jsonBuffer, n);

  Serial.print("Published: ");
  Serial.println(jsonBuffer);

  delay(2000);
}
