#define BLYNK_TEMPLATE_ID "TMPL69C990gKd"
#define BLYNK_TEMPLATE_NAME "My Smart Room"
#define BLYNK_AUTH_TOKEN "I3og4qw9lU64cDm6sMUimvRFJTJTPd0O"

#define BLYNK_PRINT Serial

#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Time.h>

#ifndef APSSID
#define APSSID "Nguyen 1"  // existing Wifi network
#define APPSK "07072011"
#endif
/* Set these to your desired credentials. */
const char* ssid = APSSID;
const char* password = APPSK;
ESP8266WebServer server(80);
MDNSResponder mdns;

//Input Devices
int cambien = D5;
int LineLightpin = D7;
int lightState = 0;
int autostatus = 1;
int customstatus = 0;
int autoControlValue = 1;
int customControlValue = 0;

//time
int timezone = 7 * 3600;
int dst = 0;

unsigned long lastResetTime = 0;

void setup() {
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  Serial.println();
  Serial.print("Connect to existing Wifi network...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(LineLightpin, OUTPUT);
  pinMode(cambien, INPUT);

  lastResetTime = millis();  // Lưu thời gian khi khởi động

  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
  Serial.println("Waiting for server!");
  while (!time(nullptr)) {
    Serial.print("#");
    delay(500);
  }
  Serial.println("Time respone is ok!");

  server.begin();
  Serial.println("HTTP server started");

  Blynk.virtualWrite(V1, 1);
  Blynk.virtualWrite(V2, 0);
}

void loop() {
  // Serial.print("IP host: ");
  // Serial.println(ssid);
  // Serial.println(password);
  Blynk.run();

  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  int currentHour = p_tm->tm_hour;
  int currentMinute = p_tm->tm_min;
  // Serial.print("Time: ");
  // Serial.print(currentHour);
  // Serial.print(":");
  // Serial.print(currentMinute);
  // Serial.print(" - ");
  // Serial.print("AutoStatus: ");
  // Serial.print(autostatus);
  // Serial.print(" - ");
  // Serial.print("CustomStatus: ");
  // Serial.print(customstatus);
  // Serial.print(" - ");
  // Serial.print("Light: ");
  // Serial.println(lightState);

  if (autostatus)
    Blynk.virtualWrite(V3, "        On        ");   //text V3 On
  else
    Blynk.virtualWrite(V3, "        Off        ");   //text V3 Off

  if (customstatus)
    Blynk.virtualWrite(V4, "        On        ");   //text V4 On
  else
    Blynk.virtualWrite(V4, "        Off        ");    //text V4 Off

  // kiểm tra giá tị Auto 
  if (autoControlValue == 1) {    // Nếu giá trị Auto đang mở thì nút Custome chuyển trạng thái OFF
    // Blynk.virtualWrite(V1, 1);
    Blynk.virtualWrite(V2, 0);
    // Serial.print("checkRequest: ");
    checkRequest();
  } else {
    // Blynk.virtualWrite(V2, 0);
    // Serial.print("checkLight: ");
    checkLight();
  }

  if (customControlValue == 1) {
    Blynk.virtualWrite(V1, 0);
    // Serial.print("customLight: ");
    customLight();
  }

  if ((customControlValue == 0 && autoControlValue == 0 && lightState == 1)) {
    Blynk.virtualWrite(V1, 0);
    Blynk.virtualWrite(V2, 0);
    digitalWrite(LineLightpin, LOW);
    lightState = 0;
  }

  // if((customControlValue == 1 && autoControlValue == 1 && lightState == 0) || (customControlValue == 1 && autoControlValue == 1 && lightState == 1))
  // {
  //   Blynk.virtualWrite(V1, 0);
  //   Blynk.virtualWrite(V2, 0);
  //   digitalWrite(LineLightpin, LOW);
  //   lightState = 0;
  //   autostatus = 0;
  //   customstatus = 0;
  // }

  if (millis() - lastResetTime > 24 * 60 * 60 * 1000) {
    resetDevice();  // Reset thiết bị
  }
}

BLYNK_WRITE(V1) {
  Blynk.virtualWrite(V2, 0);
  customstatus = 0;
  autoControlValue = param.asInt();
  autostatus = autoControlValue;
  // Blynk.virtualWrite(V2, 0);
}

// Hàm kiểm tra giá trị thời gian và cảm biến
void checkRequest() {
  // checkRequest();
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  int currentHour = p_tm->tm_hour;
  int currentMinute = p_tm->tm_min;

  int value = digitalRead(cambien);     // Doc cam bien anh sang.
  if (currentHour >= 19) {  // Kiem tra dung gio thi mo den.
    if (value == 1) {     // Kiem tra gia tri cam bien anh sang. Neu bang 1 thì den mo.
      digitalWrite(LineLightpin, HIGH);
      lightState = 1;
    } else {
      digitalWrite(LineLightpin, LOW);
      lightState = 0;
    }
  } else if (currentHour >= 0 && currentHour < 6) {
    if (value == 1) {
      digitalWrite(LineLightpin, HIGH);
      lightState = 1;
    } else {
      digitalWrite(LineLightpin, LOW);
      lightState = 0;
    }
  } else if (currentHour >= 6) {      // Kiem tra neu dung gio thi tat den
    digitalWrite(LineLightpin, LOW);
    lightState = 0;
  }
}

void checkLight() {
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  int currentHour = p_tm->tm_hour;
  int currentMinute = p_tm->tm_min;

  if (currentHour >= 6 && currentMinute == 00) {
    digitalWrite(LineLightpin, LOW);
    lightState = 0;
  }
}

BLYNK_WRITE(V2) {
  customControlValue = param.asInt();
  customstatus = customControlValue;
  // Blynk.virtualWrite(V1, 0);
}

void customLight() {
  if (customControlValue == 1) {
    digitalWrite(LineLightpin, HIGH);  // Bật đèn
    lightState = 1;                    // Cập nhật trạng thái của đèn
  } else {
    digitalWrite(LineLightpin, LOW);  // Tắt đèn
    lightState = 0;                   // Cập nhật trạng thái của đèn
  }
}

void resetDevice() {
  // Serial.println("Resetting device...");
  ESP.restart();  // Gọi hàm restart để reset thiết bị
}