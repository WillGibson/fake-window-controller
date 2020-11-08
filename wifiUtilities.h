#include <SPI.h>
#include <WiFiNINA.h>

void checkWifiStatus() {
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true); // don't continue
  }
}

void checkFirmwareVersion() {
  String wifiFirmwareVersion = WiFi.firmwareVersion();
  if (wifiFirmwareVersion < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware from " + wifiFirmwareVersion + " to " + (String)WIFI_FIRMWARE_LATEST_VERSION);
  }
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

void printWiFiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP address : ");
  Serial.println(ip);

  Serial.print("Subnet mask: ");
  Serial.println((IPAddress)WiFi.subnetMask());

  Serial.print("Gateway IP : ");
  Serial.println((IPAddress)WiFi.gatewayIP());

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI): ");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type: ");
  Serial.println(encryption, HEX);

  Serial.println();
}

void connectToWifi(char ssid[], char pass[]) {
  int status = WL_IDLE_STATUS;
  Serial.println();
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.print(ssid);
  int timeout = 0;
  while (status != WL_CONNECTED && timeout < 100) {
    Serial.print(".");
    status = WiFi.begin(ssid, pass);
    delay(100);
  }
  Serial.println();
  Serial.println();
  
  if (status != WL_CONNECTED) {
    Serial.println("Could not connect to the WiFi network");
    Serial.println("Effectively exiting"); while (true);
  }
  
  Serial.println("You're connected to the network");
  printCurrentNet();
  printWiFiData();
}
