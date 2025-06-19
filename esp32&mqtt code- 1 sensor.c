#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttClientId = "ruqaya-simple-esp";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Topic: ");
  Serial.println(topic);

  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup_wifi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("🔁 Connecting to MQTT...");
    if (client.connect(mqttClientId)) {
      Serial.println("✅ connected");

      client.subscribe("ruqaya/sensors/temp555");
    } else {
      Serial.print("❌ failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  client.subscribe("ruqaya/sensors/temp555");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 

# random readings
  float temp = random(250, 350) / 10.0;
  char msg[10];
  dtostrf(temp, 4, 2, msg);

  client.publish("ruqaya/sensors/temp555", msg);

  Serial.print(" Sent: ");
  Serial.println(msg);

  delay(3000);
}
