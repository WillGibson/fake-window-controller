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

  if (pingResult >= 0) {
    Serial.print("SUCCESS! RTT = ");
    Serial.print(pingResult);
    Serial.println(" ms");
  } else {
    Serial.print("FAILED! Error code: ");
    Serial.println(pingResult);
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

    delay(1000);

    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
  } else {
    Serial.println("Could not connect to host");
  }
}
