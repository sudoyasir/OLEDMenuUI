#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <EEPROM.h>
#include "MenuUI.h"

// OLED configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pins
#define BUTTON_UP     2
#define BUTTON_DOWN   3
#define BUTTON_SELECT 4
#define BUTTON_BACK   5
#define BUZZER_PIN    6
#define DHTPIN        7
#define DHTTYPE       DHT11
#define TRIG_PIN      8
#define ECHO_PIN      9

// Components
DHT dht(DHTPIN, DHTTYPE);
MenuUI menu(&display, BUTTON_UP, BUTTON_DOWN, BUTTON_SELECT, BUTTON_BACK, BUZZER_PIN);

// EEPROM addresses
#define EEPROM_SOUND_ADDR 0
#define EEPROM_BRIGHT_ADDR 1

// State tracking
bool soundOn = true;
bool brightnessHigh = true;

// Menu state
enum ScreenState {
  SCREEN_MENU,
  SCREEN_TEMP,
  SCREEN_HUMID,
  SCREEN_DIST,
  SCREEN_ABOUT
};
ScreenState currentScreen = SCREEN_MENU;
unsigned long lastUpdate = 0;

// Menu items
const char* mainMenu[] = {
  "Temperature",
  "Humidity",
  "Distance",
  "Settings",
  "About"
};

const char* settingsMenu[] = {
  "Sound: ON",
  "Brightness: HIGH"
};

void updateSettingsMenuLabels() {
  settingsMenu[0] = soundOn ? "Sound: ON" : "Sound: OFF";
  settingsMenu[1] = brightnessHigh ? "Brightness: HIGH" : "Brightness: LOW";
}

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  dht.begin();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Read EEPROM settings
  soundOn = EEPROM.read(EEPROM_SOUND_ADDR) == 1;
  brightnessHigh = EEPROM.read(EEPROM_BRIGHT_ADDR) == 1;

  // Apply settings
  menu.setBeepEnabled(soundOn);
  menu.setBrightnessLevel(brightnessHigh ? 255 : 10);

  menu.begin();
  updateSettingsMenuLabels();
  menu.setMenuItems(mainMenu, 5);
  menu.setCallback(onMainSelect);
  menu.setBackCallback(nullptr);
}

void loop() {
  if (currentScreen == SCREEN_MENU) {
    menu.update();
  } else {
    updateScreen();
    if (digitalRead(BUTTON_BACK) == LOW) {
      delay(200); // simple debounce
      backToMain();
      currentScreen = SCREEN_MENU;
    }
  }
}

void updateScreen() {
  unsigned long now = millis();
  if (now - lastUpdate < 1000) return;
  lastUpdate = now;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  if (currentScreen == SCREEN_TEMP) {
    float t = dht.readTemperature();
    display.setCursor(0, 0);
    display.print("Temp (C)");
    display.setCursor(0, 16);
    display.setTextSize(2);
    display.print(isnan(t) ? 0 : t);

  } else if (currentScreen == SCREEN_HUMID) {
    float h = dht.readHumidity();
    display.setCursor(0, 0);
    display.print("Humidity (%)");
    display.setCursor(0, 16);
    display.setTextSize(2);
    display.print(isnan(h) ? 0 : h);

  } else if (currentScreen == SCREEN_DIST) {
    long duration, distance;
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration * 0.034 / 2;

    display.setCursor(0, 0);
    display.print("Distance (cm)");
    display.setCursor(0, 16);
    display.setTextSize(2);
    display.print(distance);

  } else if (currentScreen == SCREEN_ABOUT) {
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("OLED Menu UI V1");
    display.setCursor(0, 16);
    display.println("@sudoyasir");
  }

  display.display();
}

void backToMain() {
  updateSettingsMenuLabels();
  menu.setMenuItems(mainMenu, 5, false);
  menu.setCallback(onMainSelect);
  menu.setBackCallback(nullptr);
}

void backToSettings() {
  updateSettingsMenuLabels();
  menu.setMenuItems(settingsMenu, 2, false);
  menu.setCallback(onSettingsSelect);
  menu.setBackCallback(backToMain);
}

void onMainSelect(int index) {
  switch (index) {
    case 0: currentScreen = SCREEN_TEMP; break;
    case 1: currentScreen = SCREEN_HUMID; break;
    case 2: currentScreen = SCREEN_DIST; break;
    case 3:
      updateSettingsMenuLabels();
      menu.setMenuItems(settingsMenu, 2);
      menu.setCallback(onSettingsSelect);
      menu.setBackCallback(backToMain);
      break;
    case 4: currentScreen = SCREEN_ABOUT; break;
  }
  lastUpdate = 0; // force refresh
}

void onSettingsSelect(int index) {
  if (index == 0) {
    soundOn = !soundOn;
    EEPROM.write(EEPROM_SOUND_ADDR, soundOn ? 1 : 0);
    menu.setBeepEnabled(soundOn);
  } else if (index == 1) {
    brightnessHigh = !brightnessHigh;
    EEPROM.write(EEPROM_BRIGHT_ADDR, brightnessHigh ? 1 : 0);
    menu.setBrightnessLevel(brightnessHigh ? 255 : 10);
  }
  updateSettingsMenuLabels();
  menu.setMenuItems(settingsMenu, 2, false);
}
