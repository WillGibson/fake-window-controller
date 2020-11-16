#include "getWeather.h"
#include "wifiUtilities.h"

#define WEATHER_LOCATION "bristol,UK"
#define WINDOW_BRIGHTNESS_PIN 3
#define MIN_BRIGHTNESS 0 // 0 - 100
#define MAX_BRIGHTNESS 75 // 0 - 100
#define ZERO_OFFSET 16 // No light comes out below this so stay above this
#define SUNLIGHT_TRANSITION_TIME 2400 // seconds
#define INTERVAL 60000 // milliseconds
#define DEBUG 0 // 0 or 1

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
}

void loop() {

  weather = getWeather(apiKey, WEATHER_LOCATION);

  long now = long(weather["now"]);
  long sunrise = long(weather["sunrise"]);
  long sunset = long(weather["sunset"]);
  long sunriseStartTime = sunrise - (SUNLIGHT_TRANSITION_TIME / 2);
  long sunriseEndTime = sunrise + (SUNLIGHT_TRANSITION_TIME / 2);
  long sunsetStartTime = sunset - (SUNLIGHT_TRANSITION_TIME / 2);
  long sunsetEndTime = sunset + (SUNLIGHT_TRANSITION_TIME / 2);
  long cloudiness = long(weather["cloudiness"]);

  int daytimeLightLevel = MAX_BRIGHTNESS - (cloudiness / 3);
  int nighttimeLightLevel = adjustedMinBrightness;
  int lightLevelDifference = daytimeLightLevel - nighttimeLightLevel;
  Serial.println((String)"now: " + now);
  Serial.println((String)"sunrise: " + sunrise);
  Serial.println((String)"sunset: " + sunset);
  Serial.println((String)"sunriseStartTime: " + sunriseStartTime);
  Serial.println((String)"sunriseEndTime: " + sunriseEndTime);
  Serial.println((String)"sunsetStartTime: " + sunsetStartTime);
  Serial.println((String)"sunsetEndTime: " + sunsetEndTime);
  Serial.println((String)"cloudiness: " + cloudiness);

  Serial.println((String)"daytimeLightLevel: " + daytimeLightLevel);
  Serial.println((String)"nighttimeLightLevel: " + nighttimeLightLevel);
  Serial.println((String)"lightLevelDifference: " + lightLevelDifference);
  Serial.println();

  if (sunriseEndTime <= now && now <= sunsetStartTime) {
    Serial.println("Daytime");
    currentLightLevel = daytimeLightLevel;
  } else if (sunriseStartTime < now && now < sunriseEndTime) {
    Serial.println("Sunrise");
    float howFarFromDarkToLight = (float(now - sunriseStartTime) / float(SUNLIGHT_TRANSITION_TIME));
    currentLightLevel = nighttimeLightLevel + (lightLevelDifference * howFarFromDarkToLight);
  } else if (sunsetStartTime < now && now < sunsetEndTime) {
    Serial.println("Sunset");
    float howFarFromLightToDark = (float(now - sunsetStartTime) / float(SUNLIGHT_TRANSITION_TIME));
    currentLightLevel = daytimeLightLevel - (lightLevelDifference * howFarFromLightToDark);
  } else {
    Serial.println("Nighttime");
    currentLightLevel = nighttimeLightLevel;
  }
  Serial.println();
  
  currentLightLevelForPin = min(
                              255,
                              pow((float)(currentLightLevel / 100) * cubeRootOf255, 3)
                            );
  Serial.println((String)"currentLightLevel: " + currentLightLevel);
  Serial.println((String)"currentLightLevelForPin: " + currentLightLevelForPin);

  analogWrite(WINDOW_BRIGHTNESS_PIN, currentLightLevelForPin);

  // Todo: This could be longer when we're well outside sunrise/sunset times
  delay(INTERVAL);
}

void log(String message) {
  if (DEBUG == 1) Serial.println(message);
}
