#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Thông tin WiFi
const char* ssid = "Trung Tam TT-TV T3";
const char* password = "12345679";

// Thông tin MQTT
const char* mqtt_server = "192.168.22.76"; // Địa chỉ MQTT broker (máy tính Flask)
const int mqtt_port = 1883;
const char* mqtt_topic = "home/test"; // Topic nhận và gửi dữ liệu từ Flask
const char* mqtt_topic_sub = "home/sub";

// WiFi và MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

// Hàm callback khi nhận dữ liệu từ MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  // Serial.print("Message content: ");
  // Serial.print(length);
  String data = "";
  for (int i = 0; i < length; i++) {
    data+=(char)payload[i];  // In từng ký tự từ payload
  }
  Serial.println(data);  // Xuống dòng sau khi in xong
}


void setup() {
  Serial.begin(9600);
  Serial.println("Init");
  WiFi.begin(ssid, password);

  // Kết nối WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Kết nối MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);  // Gán callback xử lý dữ liệu nhận được từ topic
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client")) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(mqtt_topic_sub); // Đăng ký nhận dữ liệu từ topic
    } else {
      Serial.print("MQTT connection failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void loop() {
  // Kiểm tra và duy trì kết nối MQTT
  if (!client.connected()) {
    while (!client.connected()) {
      Serial.println("Reconnecting to MQTT...");
      if (client.connect("ESP8266Client")) {
        Serial.println("Reconnected");
        client.subscribe(mqtt_topic_sub); // Đăng ký lại topic nếu bị mất kết nối
      } else {
        delay(2000);
      }
    }
  }
  
  // Chạy vòng lặp xử lý MQTT
  client.loop();

  // Gửi dữ liệu từ Serial lên MQTT
  String payload1 = Serial.readStringUntil('\n');
  if (payload1 != "") {
    client.publish(mqtt_topic, payload1.c_str()); // Gửi payload đến topic
  }
}
