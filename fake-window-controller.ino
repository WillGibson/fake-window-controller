#include "getWeather.h"
#include "wifiUtilities.h"

#define WEATHER_LOCATION "torino,IT"
#define WINDOW_BRIGHTNESS_PIN 3
#define MIN_BRIGHTNESS 0 // 0 - 100
#define MAX_BRIGHTNESS 75 // 0 - 100
#define BRIGHTNESS_INCREMENT_SIZE 2
#define FADE_STEP_INTERVAL 50
#define ZERO_OFFSET 16 // No light comes out below this so stay above this
#define DEBUG 1 // 0 or 1

#include "secrets.h"
char wifiSSID[] = WIFI_SSID;
char wifiPass[] = WIFI_PASS;
char apiKey[] = OPEN_WEATHER_API_KEY;
int status = WL_IDLE_STATUS;

float desiredLightLevel;
float adjustedMinBrightness;
float currentLightLevel;
float currentLightLevelForPin;
float cubeRootOf255 = 6.34;
unsigned long timeForNextFadeStep;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("\n\n\n##########\n# REBOOT\n\n");

  checkWifiStatus();

  checkFirmwareVersion();

  connectToWifi(wifiSSID, wifiPass);
  Serial.println();

  if (DEBUG != 1) Serial.end();

  adjustedMinBrightness = MIN_BRIGHTNESS + ZERO_OFFSET;
  desiredLightLevel = MAX_BRIGHTNESS;
  currentLightLevel = adjustedMinBrightness;

  getWeather(apiKey, WEATHER_LOCATION);
}

void loop() {

//  getWeather(apiKey, WEATHER_LOCATION);

  if (currentLightLevel < desiredLightLevel) currentLightLevel += BRIGHTNESS_INCREMENT_SIZE;
  else if (currentLightLevel > desiredLightLevel) currentLightLevel -= BRIGHTNESS_INCREMENT_SIZE;

  currentLightLevelForPin = min(
                              255,
                              pow((float)(currentLightLevel / 100) * cubeRootOf255, 3)
                            );
//  log(
//    (String)"currentLightLevel // currentLightLevelForPin: " +
//    currentLightLevel + " // " + currentLightLevelForPin
//  );

  if (currentLightLevel >= MAX_BRIGHTNESS) desiredLightLevel = adjustedMinBrightness;
  if (currentLightLevel <= adjustedMinBrightness) desiredLightLevel = MAX_BRIGHTNESS;
  analogWrite(WINDOW_BRIGHTNESS_PIN, currentLightLevelForPin);

  delay(FADE_STEP_INTERVAL);
}

void log(String message) {
  if (DEBUG == 1) Serial.println(message);
}
