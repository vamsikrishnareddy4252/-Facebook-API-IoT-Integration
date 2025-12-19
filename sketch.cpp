#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "DHT.h"

// ======================= CONFIGURATION =======================
// 1. Wi-Fi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// 2. Facebook Keys (PASTE YOURS HERE)
String pageId = "858676340668355"; 
String accessToken = "";

// 3. Sensor Settings
#define DHTPIN 15     
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

// 4. Timer (Post every 10 seconds for testing)
unsigned long lastPostTime = 0;
const long postInterval = 15000; 
// ==============================================================

void postToFacebook(String message); 

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Connected to WiFi!");
}

void loop() {
  if (millis() - lastPostTime > postInterval) {
    lastPostTime = millis(); 

    // 1. Read BOTH Temperature and Humidity
    float t = dht.readTemperature();
    float h = dht.readHumidity(); // <--- NEW LINE
    
    // 2. Check if EITHER reading failed
    // The "||" symbol means "OR"
    if (isnan(t) || isnan(h)) {
      Serial.println("❌ Failed to read sensor!");
      return;
    }

    Serial.print("Temp: ");
    Serial.print(t);
    Serial.print("°C | Humidity: "); // Print locally to see it
    Serial.print(h);
    Serial.println("%");

    // 3. Create a combined message for Facebook
    // We add the humidity to the string here
    String msg = "Live from Wokwi! Temp: " + String(t, 1) + "°C, Humidity: " + String(h, 1) + "%";
    
    postToFacebook(msg);
  }
}

void postToFacebook(String message) {
  WiFiClientSecure client;
  HTTPClient http;

  client.setInsecure(); 

  String url = "https://graph.facebook.com/v19.0/" + pageId + "/feed";
  
  Serial.println("📤 Sending to Facebook...");
  
  if (http.begin(client, url)) {
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String payload = "message=" + message + "&access_token=" + accessToken;
    
    int httpCode = http.POST(payload);
    
    if (httpCode == 200) {
      Serial.println("✅ SUCCESS! Posted to Facebook.");
      Serial.println(httpCode);
      Serial.println(http.getString());
    } else {
      Serial.print("❌ Error: ");
      Serial.println(httpCode);
      Serial.println(http.getString());
    }
    http.end();
  } else {
    Serial.println("❌ Connection failed");
  }//EAAbvDReI6poBQGZBhoOYGD6uLAgntkaLs4H4qtMAGm2sErcVBKJfRxcCeyHC9vdsTmrp4tXmUxx3MSfZBwVYYqZAEZCd53ohZAxWZBZAEKbE113Hi5s1ggAiU7ZBvoHnSswy2p4i8ZB21n9jyINRMhKr2beeJMGJYLZBbWcKatGbb6KCEWGi1tRgichb5sLMT7ZCzCRCYi
}