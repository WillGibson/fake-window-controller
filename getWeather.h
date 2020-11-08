#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>

WiFiSSLClient client;

void getWeather(String apiKey, String location) {

  char host[] = "api.openweathermap.org";
  String path = "/data/2.5/forecast?q=torino,IT&cnt=3&appid=" + apiKey;

  //  char host[] = "example.com";
  //  String path = "/";

  int pingResult;
  pingResult = WiFi.ping(host);

  if (WiFi.ping(host) >= 0) {
    Serial.print("Successfully pinged ");
    Serial.print(host);
    Serial.print(" in ");
    Serial.print(pingResult);
    Serial.println("ms");
  } else {
    Serial.print("Could not ping ");
    Serial.println(host);
  }


  Serial.println();
  Serial.print("Starting connection to ");
  Serial.println(host);

  if (client.connect(host, 443)) {

    Serial.println("connected to host");
    // Make a HTTP request:
    client.println("GET " + path + " HTTP/1.1");
    client.println("Host: api.openweathermap.org");
    client.println("Connection: close");
    if (client.println() == 0) {
      Serial.println(F("Failed to send request"));
      return;
    }

    // Check HTTP status
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
      Serial.print(F("Unexpected response: "));
      Serial.println(status);
      return;
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders)) {
      Serial.println(F("Invalid response"));
      return;
    }

    Serial.print("Waiting for response");
    int timeout = 0;
    while (!client.available() && timeout < 1000) {
      Serial.print(".");
      delay(50);
    }
    Serial.println();
    if (!client.available()) {
      Serial.println("No response received");
      return;
    }

    // Allocate the JSON document
    const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 5000;
    DynamicJsonDocument doc(capacity);

    // Parse JSON object
    DeserializationError error = deserializeJson(doc, client);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    client.stop();

//    serializeJson(doc, Serial);
  } else {
    Serial.println("Could not connect to host");
  }
}