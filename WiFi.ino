#include <ESP8266WiFi.h>  // WiFi library
#include <WiFiUdp.h>      // UPD functionality

// Wi-Fi Credentials
const char WIFI_SSID[] = "network";
const char WIFI_PASSWORD[] = "password";

// UDP variables
WiFiUDP udp;
unsigned int localUdpPort = 8080;
char incomingPacket[255];
const char broadcastMessage[] = "ESP8266 UDP Server Discovery Broadcast";
const char serverAcknowledgementMessage[] = "NodeJs Server Acknowledgement Message";

// Server details - written to when the server is found
IPAddress serverIp;
unsigned int serverPort;

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
  #if LOGGING
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  #endif LOGGING
}

// Discover the server via a UDP broadcast, and store it's IP and Port in the local network in field variables for later use
void discoverServer()
{
  changeColor(PURPLE, false); // change the color of the RGB status LED to signal that the program is searching for the server
  
  bool serverFound = false; // stop when the server is found

  IPAddress broadcastIp = ~WiFi.subnetMask() | WiFi.gatewayIP(); // Get the Broadcast IP of the local network (e.g. 192.168.0.255)
    
  #if LOGGING
  Serial.printf("Sending UDP Broadcast on IP: %s, Port: %d, Message: %s\n", broadcastIp.toString().c_str(), localUdpPort, broadcastMessage);
  #endif
  
  while (!serverFound)
  {
    // Send UDP Broadcast
    udp.beginPacket(broadcastIp, localUdpPort);
    udp.write(broadcastMessage);
    udp.endPacket();

    // Receive UDP packets
    int packetSize = udp.parsePacket();
    if (packetSize > 0)
    {
      // Read incoming UDP Packet
      int len = udp.read(incomingPacket, 255);
      if (len > 0)
      {
        incomingPacket[len] = 0;
      }

      #if LOGGING
      Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
      Serial.printf("UDP packet contents: %s\n", incomingPacket);
      #endif
  
      // Check if the received message is from the server we are searching for
      if (strcmp(incomingPacket, serverAcknowledgementMessage) == 0)
      {
        serverIp = udp.remoteIP();
        serverPort = udp.remotePort();

        #if LOGGING
        Serial.printf("Found Server on IP: %s, Port: %d\n", serverIp.toString().c_str(), serverPort);
        #endif

        serverFound = true;
        changeColor(YELLOW, false); // Change status color of RGB LED back to yellow
      }
    }
    
    delay(PAUSE_TIME_MS);
  }
}
