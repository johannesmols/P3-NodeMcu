/**
 * Author:    Johannes Mols
 * Created:   08.10.2018
 **/

#include <ESP8266WiFi.h>        // WiFi library
#include <DNSServer.h>          // DNS Server
#include <ESP8266WebServer.h>   // Web Server
#include <WiFiManager.h>        // Prompt User to Connect to WiFi: https://github.com/tzapu/WiFiManager
#include <WiFiUdp.h>            // UPD functionality
#include <ESP8266HTTPClient.h>  // HTTP Client
#include <ArduinoJson.h>        // JSON Library - use version 5.13.2, version 6 has code breaking changes

// UDP variables
WiFiUDP udp;
unsigned int localUdpPort = 8080;
char incomingPacket[255];
const char broadcastMessage[] = "ESP8266 UDP Server Discovery Broadcast";
const char serverAcknowledgementMessage[] = "NodeJs Server Acknowledgement Message"; // Not used right now, same message is just sent back

// Server details - written to when the server is found
IPAddress serverIp = ~WiFi.subnetMask() | WiFi.gatewayIP(); // Use Broadcast Address as default in case the UDP service discovery isn't working as intended
unsigned int serverPort = localUdpPort; // Use local port as default in case the UDP service discovery ins't working as intended

// Server info - info returned by the server API
String deviceId; // Assigned device ID by the server, used when interacting with the API

/*
 * Set up a soft access point for clients to connect (via phone, laptop, ...)
 * After connecting, the user will be prompted to select the WiFi network
 * that he wants to use with the NodeMcu. He will be asked for the password.
 * The access point will then be closed and the NodeMcu connects to the selected WiFi
 */
void setupWiFiDynamic(bool resetSettings)
{
  changeColor(YELLOW, false); // indicate that it is working
  
  WiFiManager wifiManager;

  if (resetSettings)
  {
    wifiManager.resetSettings();
  }
  
  wifiManager.autoConnect("Smart Door"); // Create an access point with this SSID, the library handles the rest

  #if LOGGING
  Serial.println("Connected to network");
  #endif

  startUdpListener();

  changeColor(GREEN, false); // indicate that WiFi setup was completed successfully
}

// Start listening to UDP traffic on a specific port
void startUdpListener()
{
  udp.begin(localUdpPort); // begin listening on UDP port
  #if LOGGING
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  #endif LOGGING
}

// Discover the server via a UDP broadcast, and store it's IP and Port in the local network in field variables for later use
// IMPORTANT - For the server to work, the Linux Firewall has to be disabled!!!
void discoverServer()
{
  changeColor(PURPLE, false); // change the color of the RGB status LED to signal that the program is searching for the server
  
  bool serverFound = false; // stop when the server is found

  IPAddress broadcastIp = ~WiFi.subnetMask() | WiFi.gatewayIP(); // Get the Broadcast IP of the local network (e.g. 192.168.0.255)
  
  while (!serverFound)
  {
    // Send UDP Broadcast
    udp.beginPacket(broadcastIp, localUdpPort);
    udp.write(broadcastMessage);
    udp.endPacket();
    
    #if LOGGING
    Serial.printf("[UDP] Sending UDP Broadcast on IP: %s, Port: %d, Message: %s\n", broadcastIp.toString().c_str(), localUdpPort, broadcastMessage);
    #endif

    delay(1000); // Pause a few milliseconds to avoid flooding the network

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
      Serial.printf("[UDP] Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
      Serial.printf("[UDP] Packet contents: %s\n", incomingPacket);
      #endif
  
      // Check if the received message is from the server we are searching for
      if (strcmp(incomingPacket, broadcastMessage) == 0)
      {
        serverIp = udp.remoteIP();
        serverPort = udp.remotePort();

        #if LOGGING
        Serial.printf("[UDP] Found Server on IP: %s, Port: %d\n", serverIp.toString().c_str(), serverPort);
        #endif

        serverFound = true;
        changeColor(YELLOW, false); // Change status color of RGB LED back to yellow
      }
    }
  }
}

// Use the server API to register the device and store the assigned ID
void registerDevice()
{
  changeColor(PINK, false); // change the color of the RGB status LED to signal that the device is being registered with the server

  String host = "http://" + (String) serverIp.toString().c_str() + ":" + (String) serverPort + "/api/register";

  #if LOGGING
  Serial.println("[HTTP] Sending register POST request to server: " + host);
  #endif

  // Create HTTP client and create request
  HTTPClient http;
  http.begin(host);
  http.addHeader("Content-Type", "application/json");

  // Prepare POST request by adding a message in JSON format
  vector<String> types, values;
  types.push_back("device_type");
  values.push_back(WiFi.macAddress());

  // Post request to the server and fetch response
  int httpCode = http.POST(convertValuesToJson(types, values));
  String response = http.getString();
  decodeRegisterJsonResponse(response);

  #if LOGGING
  Serial.println("[HTTP] Response Code: " + (String) httpCode);
  Serial.println("[HTTP] Request Response: " + response);
  Serial.println("Registered Device with ID: " + deviceId); 
  #endif

  changeColor(YELLOW, false); // Change status color of RGB LED back to yellow
}

// Convert a list of types and values to a Json String
String convertValuesToJson(vector<String> types, vector<String> values)
{
  if (types.size() != values.size()) // Vector sizes have to match up for a valid JSON String
  {
    Serial.println("[Error] Number of types and values don't match");
    return "";
  }

  int numberOfElements = types.size() + values.size();
  const size_t bufferSize = JSON_ARRAY_SIZE(numberOfElements) + 128; // size of dynamic buffer
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();
  for (int i = 0; i < types.size(); i++)
  {
    root[types[i]] = values[i]; // add values to the JSON Object
  }

  char jsonMessageBuffer[bufferSize];
  root.printTo(jsonMessageBuffer, sizeof(jsonMessageBuffer)); // print Json as a char array

  return (String) jsonMessageBuffer; // return the JSON as a String
}

// Decode the JSON string returned by the server upon registering the device, and store the ID to a variable
void decodeRegisterJsonResponse(String json)
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);

  const char* value = root["_id"];
  deviceId = (String) value;
}
