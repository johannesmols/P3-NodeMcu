#include <ESP8266WiFi.h>

// Wi-Fi Credentials
const char WIFI_SSID[] = "ssid";
const char WIFI_PASSWORD[] = "password";

void setupWiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  #if LOGGING
  Serial.println("Connecting to network: " + (String) WIFI_SSID);
  #endif
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
  }

  #if LOGGING
  Serial.print("Connected to network, Local IP Address: ");
  Serial.println(WiFi.localIP());
  #endif
}
