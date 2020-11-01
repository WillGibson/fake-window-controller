
// CONFIGURATION
#define WINDOW_BRIGHTNESS_PIN 3
#define MAX_BRIGHTNESS 20 // 0-255
#define MIN_BRIGHTNESS 1 // 0-255
#define FADE_INTERVAL 75 // ms between fade steps

// DECLARATIONS
byte desiredLightLevel;
byte currentLightLevel;
unsigned long timeForNextFadeStep;

void setup() {
  Serial.begin(9600);
  desiredLightLevel = MAX_BRIGHTNESS;
  Serial.println((String)"Initial desiredLightLevel: " + desiredLightLevel);
}

void loop() {
  if (millis() > timeForNextFadeStep) {
    timeForNextFadeStep = millis() + (unsigned long)FADE_INTERVAL;
    if (currentLightLevel == MAX_BRIGHTNESS) desiredLightLevel = MIN_BRIGHTNESS;
    if (currentLightLevel == MIN_BRIGHTNESS) desiredLightLevel = MAX_BRIGHTNESS;
    if (currentLightLevel < desiredLightLevel) currentLightLevel++;
    if (currentLightLevel > desiredLightLevel) currentLightLevel--;
    Serial.println((String)"desiredLightLevel: " + desiredLightLevel);
    Serial.println((String)"currentLightLevel:" + currentLightLevel);
    analogWrite(WINDOW_BRIGHTNESS_PIN, currentLightLevel);
  }
}
