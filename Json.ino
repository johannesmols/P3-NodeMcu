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

// Parse a list of tag ID's into a JSON format to be sent to the server for processing
String parseReadTagsToJson(vector<String> tags)
{
  const size_t bufferSize = JSON_ARRAY_SIZE(tags.size()) + 128; // size of dynamic buffer
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();

  // Probably add some other stuff before tags - will see when the server side is implemented
  root["mac"] = WiFi.macAddress();

  // Nested array for tag ID's
  JsonArray& tagsArray = root.createNestedArray("tags");
  for (int i = 0; i < tags.size(); i++)
  {
    tagsArray.add(tags[i]);
  }

  char jsonMessageBuffer[bufferSize];
  root.printTo(jsonMessageBuffer, sizeof(jsonMessageBuffer)); // print Json as a char array

  return (String) jsonMessageBuffer; // return the JSON as a String
}