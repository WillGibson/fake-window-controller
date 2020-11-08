#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>

WiFiSSLClient client;

void getWeather(char* apiKey, String location) {

  String host = "api.openweathermap.org";
  String path = "/data/2.5/forecast?q=torino,IT&cnt=3&appid=f955aab26dbecc5aad4287b8dc7e8d35";


  client.println();
  Serial.println("Starting connection to " + host + "...");
  // if you get a connection, report back via serial:
  if (client.connect("api.openweathermap.org", 80)) {
    Serial.println("connected to " + host);
    // Make a HTTP request:
    client.println("GET " + path + " HTTP/1.1");
    client.println("Host: " + host);
    client.println("Connection: close");
    client.println();

    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
  } else {
    Serial.println("Could not connect to " + host);
  }
}
