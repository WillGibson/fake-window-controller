#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>

WiFiSSLClient client;

StaticJsonDocument<200> getWeather(String apiKey, String location) {

  char host[] = "api.openweathermap.org";
  String path = "/data/2.5/weather?q=" + location + "&cnt=1&appid=" + apiKey;

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
  } else {
    Serial.println("Could not connect to host");
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

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  Serial.print("status: ");
  Serial.println(status);
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }

  // Get time from response headers (because the board has no clock)
  String dateHeader;
  while (client.connected()) {
    String dateHeaderKey = "Date: ";
    String line = client.readStringUntil('\n');
    if (line.indexOf(dateHeaderKey) != -1) {
      dateHeader = line.substring(sizeof(dateHeaderKey));
      Serial.println("dateHeader: " + dateHeader);
      break;
    }
  }
  if (sizeof(dateHeader) == 0) {
    Serial.print(F("Could not extract date header"));
    return;
  }
  long epochTime = 0;
  long aYearInSeconds = 31536000;
  long aDayInSeconds = 86400;
  long anHourInSeconds = 3600;
  long aMinuteInSeconds = 60;
  // Years
  int yearsSinceEpoch = (dateHeader.substring(16, 12).toInt() - 1970);
  epochTime += yearsSinceEpoch * aYearInSeconds;
  // Days
  int dayOfMonth = dateHeader.substring(7, 5).toInt();
  int monthNum;
  String monthString = dateHeader.substring(11, 8);
  if (monthString == "Jan") {
    monthNum = 1;
  } else if (monthString == "Feb") {
    monthNum = 2;
  } else if (monthString == "Mar") {
    monthNum = 3;
  } else if (monthString == "Apr") {
    monthNum = 4;
  } else if (monthString == "May") {
    monthNum = 5;
  } else if (monthString == "Jun") {
    monthNum = 6;
  } else if (monthString == "Jul") {
    monthNum = 7;
  } else if (monthString == "Aug") {
    monthNum = 8;
  } else if (monthString == "Sep") {
    monthNum = 9;
  } else if (monthString == "Oct") {
    monthNum = 10;
  } else if (monthString == "Nov") {
    monthNum = 11;
  } else if (monthString == "Dec") {
    monthNum = 12;
  }
  if (monthNum > 1) {
    epochTime += 31 * aDayInSeconds;
  }
  if (monthNum > 2) {
    epochTime += 28 * aDayInSeconds;
  }
  if (monthNum > 3) {
    epochTime += 31 * aDayInSeconds;
  }
  if (monthNum > 4) {
    epochTime += 30 * aDayInSeconds;
  }
  if (monthNum > 5) {
    epochTime += 31 * aDayInSeconds;
  }
  if (monthNum > 6) {
    epochTime += 30 * aDayInSeconds;
  }
  if (monthNum > 7) {
    epochTime += 31 * aDayInSeconds;
  }
  if (monthNum > 8) {
    epochTime += 31 * aDayInSeconds;
  }
  if (monthNum > 9) {
    epochTime += 30 * aDayInSeconds;
  }
  if (monthNum > 10) {
    epochTime += 31 * aDayInSeconds;
  }
  if (monthNum > 11) {
    epochTime += 30 * aDayInSeconds;
  }
  epochTime += (dayOfMonth - 1) * aDayInSeconds;
  // Leap years
  int numberOfLeapYears = ((yearsSinceEpoch - 2) / 4) + 1;
  // Todo/: Need to know if the current year is a leap year I think...
  epochTime += numberOfLeapYears  * aDayInSeconds;
  // Hours
  epochTime += dateHeader.substring(19, 17).toInt();
  // Minutes
  epochTime += dateHeader.substring(22, 20).toInt();
  // Seconds
  epochTime += dateHeader.substring(25, 23).toInt();
  Serial.print("Epoch time: ");
  Serial.println(epochTime);

  // Skip rest of HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }

  // Get the response body
  char response[2000];
  int pointer = 0;
  while (client.connected() || client.available()) {
    char character = client.read();
    response[pointer] = character;
    pointer++;
  }
  client.stop();

  // Deserialise the JSON
  StaticJsonDocument<2000> jsonDocument;
  DeserializationError error = deserializeJson(jsonDocument, response);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  Serial.println("Streamlining the JSON");
  StaticJsonDocument<200> tidyJSON;
  String city = jsonDocument["name"];
  String country = jsonDocument["sys"]["country"];
  tidyJSON["location"] = city + ", " + country;
  tidyJSON["sunrise"] = jsonDocument["sys"]["sunrise"];
  tidyJSON["sunset"] = jsonDocument["sys"]["sunset"];
  tidyJSON["cloudiness"] = jsonDocument["clouds"]["all"];
  tidyJSON["now"] = epochTime;

  return tidyJSON;
}
