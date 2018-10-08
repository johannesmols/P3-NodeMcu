#include <ESP8266WiFi.h>  // WiFi library
#include <WiFiUdp.h>      // UPD functionality

// Wi-Fi Credentials
const char WIFI_SSID[] = "ssid";
const char WIFI_PASSWORD[] = "password";

// UDP
WiFiUDP udp;
unsigned int localUdpPort = 8080;
char incomingPacket[255];
char replyPacket[] = "Message received";
//IPAddress address(192, 168, 188 ,255);

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

  udp.begin(localUdpPort); // begin listening on UDP port
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}

void discoverServer()
{
  bool serverFound = false;
  
  while (!serverFound)
  {
    // Send packets
    udp.beginPacket(IPAddress(192, 168, 188, 255), localUdpPort);
    udp.write("Hello");
    udp.endPacket();

    // Receive packets
    int packetSize = udp.parsePacket();
    if (packetSize > 0)
    {
      #if LOGGING
      Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
      #endif
      
      int len = udp.read(incomingPacket, 255);
      if (len > 0)
      {
        incomingPacket[len] = 0;
      }
      
      #if LOGGING
      Serial.printf("UDP packet contents: %s\n", incomingPacket);
      #endif
      
      // send back a reply, to the IP address and port we got the packet from
      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.write(replyPacket);
      udp.endPacket();
    }

    delay(100);
  }
}
