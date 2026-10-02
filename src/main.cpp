#include <Arduino.h>
#include <WiFiMulti.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// ==================== 1. ตั้งค่า Wi-Fi ====================
#define WIFI_SSID "pond"        // ชื่อ Wi-Fi
#define WIFI_PASSWORD "12345678" // รหัสผ่าน Wi-Fi

// ==================== 2. ตั้งค่า InfluxDB ====================
#define INFLUXDB_URL "http://172.20.10.3:8086"
#define INFLUXDB_TOKEN "V19p9ccRA8NGfe9n7946go7qDwciimmxmjg06yETk703f1fooN69Yy24fE0_DoBoLZR3D-v1fUdcrwn"
#define INFLUXDB_ORG "CasaOrg"
#define INFLUXDB_BUCKET "Data1"

// ตั้งค่าโซนเวลา (ประเทศไทย UTC+7)
#define TZ_INFO "ICT-7"

WiFiMulti wifiMulti;

// สร้าง Instance เชื่อมต่อ InfluxDB
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);

// สร้าง Data Point สำหรับส่งข้อมูล
Point sensorData("device_status");

void setup() {
  Serial.begin(115200);

  // เชื่อมต่อ Wi-Fi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to Wi-Fi!");

  // ซิงค์เวลาสำหรับ InfluxDB
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // ตรวจสอบการเชื่อมต่อ InfluxDB
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void loop() {
  // อ่านค่าสัญญาณ Wi-Fi ส่งเข้า InfluxDB
  sensorData.clearFields();
  sensorData.addField("rssi", WiFi.RSSI());

  if (!client.writePoint(sensorData)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  } else {
    Serial.println("Data sent to InfluxDB successfully!");
  }

  delay(5000);
}