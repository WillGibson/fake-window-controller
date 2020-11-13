#include "getWeather.h"
#include "wifiUtilities.h"

#define WEATHER_LOCATION "bristol,UK"
#define WINDOW_BRIGHTNESS_PIN 3
#define MIN_BRIGHTNESS 0 // 0 - 100
#define MAX_BRIGHTNESS 75 // 0 - 100
#define BRIGHTNESS_INCREMENT_SIZE 2
#define INTERVAL 10000 // milliseconds
#define ZERO_OFFSET 16 // No light comes out below this so stay above this
#define DEBUG 1 // 0 or 1

#include "secrets.h"
char wifiSSID[] = WIFI_SSID;
char wifiPass[] = WIFI_PASS;
String apiKey = OPEN_WEATHER_API_KEY;
int status = WL_IDLE_STATUS;

float desiredLightLevel;
float adjustedMinBrightness;
float currentLightLevel;
float currentLightLevelForPin;
float cubeRootOf255 = 6.34;
unsigned long timeForNextFadeStep;
StaticJsonDocument<200> weather;

void setup() {
  Serial.begin(9600);
  while (!Serial) continue;
  Serial.print("\n\n##########\n# REBOOT\n");

  checkWifiStatus();

  checkFirmwareVersion();

  connectToWifi(wifiSSID, wifiPass);
  Serial.println();

  if (DEBUG != 1) Serial.end();

  adjustedMinBrightness = MIN_BRIGHTNESS + ZERO_OFFSET;
  //  desiredLightLevel = MAX_BRIGHTNESS;
  //  currentLightLevel = adjustedMinBrightness;
}

void loop() {

  weather = getWeather(apiKey, WEATHER_LOCATION);
  Serial.println();
  serializeJsonPretty(weather, Serial);
  Serial.println(); Serial.println();

  long sunrise = int(weather["sunrise"]);
  long sunset = int(weather["sunset"]);
  long cloudiness = int(weather["cloudiness"]);
  long now = int(weather["now"]);

  int daytimeLightLevel = MAX_BRIGHTNESS - (cloudiness / 2);
  int nighttimeLightLevel = adjustedMinBrightness;
  
  if (sunrise < now && now < sunset) {
    currentLightLevel = daytimeLightLevel;
  } else {
    currentLightLevel = nighttimeLightLevel;
  }

  currentLightLevelForPin = min(
                              255,
                              pow((float)(currentLightLevel / 100) * cubeRootOf255, 3)
                            );
  log(
    (String)"currentLightLevel // currentLightLevelForPin: " +
    currentLightLevel + " // " + currentLightLevelForPin
  );

  analogWrite(WINDOW_BRIGHTNESS_PIN, currentLightLevelForPin);

  delay(INTERVAL);
}

void log(String message) {
  if (DEBUG == 1) Serial.println(message);
}
