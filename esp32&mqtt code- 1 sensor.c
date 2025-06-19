#include <WiFi.h>
#include <PubSubClient.h>

// ----------- إعدادات الشبكة والـ MQTT -----------
const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttClientId = "ruqaya-simple-esp";

// ----------- الاتصال بالـ WiFi -----------
WiFiClient espClient;
PubSubClient client(espClient);

// ----------- دالة استلام الرسائل من MQTT -----------
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("📥 Topic: ");
  Serial.println(topic);

  Serial.print("💬 Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// ----------- الاتصال بالـ WiFi -----------
void setup_wifi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected");
}

// ----------- إعادة الاتصال بالـ MQTT Broker -----------
void reconnect() {
  while (!client.connected()) {
    Serial.print("🔁 Connecting to MQTT...");
    if (client.connect(mqttClientId)) {
      Serial.println("✅ connected");

      // ✅ اشتراك في التوبيك من جديد
      client.subscribe("ruqaya/sensors/temp555");
    } else {
      Serial.print("❌ failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// ----------- إعداد النظام -----------
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  // ✅ نعيد الاشتراك من هنا كمان احتياطي
  client.subscribe("ruqaya/sensors/temp555");
}

// ----------- الحلقة الرئيسية -----------
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();  // 👈 لازم تكون دايمًا موجودة علشان يستقبل

  // ✅ حرارة عشوائية بين 25 و 35
  float temp = random(250, 350) / 10.0;
  char msg[10];
  dtostrf(temp, 4, 2, msg);

  // ✅ إرسال الحرارة إلى التوبيك
  client.publish("ruqaya/sensors/temp555", msg);

  Serial.print("📤 Sent: ");
  Serial.println(msg);

  delay(3000);
}
