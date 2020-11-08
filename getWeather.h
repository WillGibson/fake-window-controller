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
  // if you get a connection, report back via serial:
  if (client.connect(host, 443)) {
    Serial.println("connected to host");
    // Make a HTTP request:
    client.println("GET " + path + " HTTP/1.1");
    client.println("Host: api.openweathermap.org");
    client.println("Connection: close");
    client.println();

    Serial.print("Waiting for response");
    int timeout = 0;
    while (!client.available() && timeout < 100) {
      Serial.print(".");
      delay(50);
    }
    Serial.println();
    if (!client.available()) {
      Serial.println("No response recieved");
      Serial.println("Effectively exiting"); while (true);
    }

    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
  } else {
    Serial.println("Could not connect to host");
  }
}
