// CONFIGURATION
#define WINDOW_BRIGHTNESS_PIN 3
#define MIN_BRIGHTNESS 0 // 0 - 100
#define MAX_BRIGHTNESS 100 // 0 - 100
#define BRIGHTNESS_INCREMENT_SIZE 2
#define FADE_STEP_INTERVAL 50
#define ZERO_OFFSET 15 // No light comes out below this so don't use it
#define DEBUG 0 // 0 or 1

// DECLARATIONS
float desiredLightLevel;
float adjustedMinBrightness;
float currentLightLevel;
float currentLightLevelForPin;
float cubeRootOf255 = 6.34;
unsigned long timeForNextFadeStep;

void setup() {
  if (DEBUG == 1) Serial.begin(9600);
  adjustedMinBrightness = MIN_BRIGHTNESS + ZERO_OFFSET;
  desiredLightLevel = MAX_BRIGHTNESS;
  currentLightLevel = adjustedMinBrightness;
}

void loop() {
  if (millis() > timeForNextFadeStep) {
    timeForNextFadeStep = millis() + (unsigned long)FADE_STEP_INTERVAL;

    if (currentLightLevel < desiredLightLevel) currentLightLevel += BRIGHTNESS_INCREMENT_SIZE;
    else if (currentLightLevel > desiredLightLevel) currentLightLevel -= BRIGHTNESS_INCREMENT_SIZE;

    currentLightLevelForPin = min(
                                255,
                                pow((float)(currentLightLevel / 100) * cubeRootOf255, 3)
                              );
    log(
      (String)"currentLightLevel // currentLightLevelForPin: " +
      currentLightLevel + " // " + currentLightLevelForPin
    );

    if (currentLightLevel >= MAX_BRIGHTNESS) desiredLightLevel = adjustedMinBrightness;
    if (currentLightLevel <= adjustedMinBrightness) desiredLightLevel = MAX_BRIGHTNESS;
    analogWrite(WINDOW_BRIGHTNESS_PIN, currentLightLevelForPin);
  }
}

void log(String message) {
  if (DEBUG == 1) Serial.println(message);
}
