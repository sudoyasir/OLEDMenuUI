
// SensorMenu Example for OLEDMenuUI Library
// Demonstrates dynamic and nested menu usage with sensor readings and settings
// Author: Yasir N. | https://github.com/sudoyasir/OLEDMenuUI

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <EEPROM.h>
#include "MenuUI.h"

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin assignments
#define BUTTON_UP     2      // Up button
#define BUTTON_DOWN   3      // Down button
#define BUTTON_SELECT 4      // Select button
#define BUTTON_BACK   5      // Back button
#define BUZZER_PIN    6      // Buzzer output
#define DHTPIN        7      // DHT sensor pin
#define DHTTYPE       DHT11  // DHT sensor type
#define TRIG_PIN      8      // Ultrasonic trigger
#define ECHO_PIN      9      // Ultrasonic echo

// Hardware components
DHT dht(DHTPIN, DHTTYPE); // Temperature/humidity sensor
MenuUI menu(&display, BUTTON_UP, BUTTON_DOWN, BUTTON_SELECT, BUTTON_BACK, BUZZER_PIN); // Menu system

// EEPROM addresses for persistent settings
#define EEPROM_SOUND_ADDR 0
#define EEPROM_BRIGHT_ADDR 1

// State variables for settings
bool soundOn = true;
bool brightnessHigh = true;

// Tracks which screen is currently displayed
enum ScreenState {
  SCREEN_MENU,
  SCREEN_TEMP,
  SCREEN_HUMID,
  SCREEN_DIST,
  SCREEN_ABOUT
};
ScreenState currentScreen = SCREEN_MENU;
unsigned long lastUpdate = 0;



// (Legacy) static settings menu for backward compatibility
const char* settingsMenu[] = {
  "Sound: ON",
  "Brightness: HIGH"
};

MenuUI::MenuItem* settingsMenuItem = nullptr;

// Dynamic menu item pointers for runtime menu structure
MenuUI::MenuItem* rootMenu = nullptr;
MenuUI::MenuItem* sensorsMenu = nullptr;
MenuUI::MenuItem* aboutMenuItem = nullptr;
MenuUI::MenuItem* soundMenuItem = nullptr;
MenuUI::MenuItem* brightnessMenuItem = nullptr;

// Menu action callbacks for sensor and about screens
void showTemp() { currentScreen = SCREEN_TEMP; lastUpdate = 0; }
void showHumid() { currentScreen = SCREEN_HUMID; lastUpdate = 0; }
void showDist() { currentScreen = SCREEN_DIST; lastUpdate = 0; }
void showAbout() { currentScreen = SCREEN_ABOUT; lastUpdate = 0; }

// Toggle sound setting and update menu label
void toggleSound() {
  soundOn = !soundOn;
  EEPROM.write(EEPROM_SOUND_ADDR, soundOn ? 1 : 0);
  menu.setBeepEnabled(soundOn);
  soundMenuItem->label = soundOn ? "Sound: ON" : "Sound: OFF";
  menu.drawMenu();
}

// Toggle brightness setting and update menu label
void toggleBrightness() {
  brightnessHigh = !brightnessHigh;
  EEPROM.write(EEPROM_BRIGHT_ADDR, brightnessHigh ? 1 : 0);
  menu.setBrightnessLevel(brightnessHigh ? 255 : 10);
  brightnessMenuItem->label = brightnessHigh ? "Brightness: HIGH" : "Brightness: LOW";
  menu.drawMenu();
}

// Build the dynamic menu structure at runtime
void buildDynamicMenu() {
  menu.clearMenu();
  rootMenu = menu.addMenuItem("ROOT");
  // Sensors submenu
  sensorsMenu = menu.addMenuItem("Sensors", nullptr, rootMenu);
  menu.addMenuItem("Temperature", showTemp, sensorsMenu);
  menu.addMenuItem("Humidity", showHumid, sensorsMenu);
  menu.addMenuItem("Distance", showDist, sensorsMenu);
  // Settings submenu
  settingsMenuItem = menu.addMenuItem("Settings", nullptr, rootMenu);
  soundMenuItem = menu.addMenuItem(soundOn ? "Sound: ON" : "Sound: OFF", toggleSound, settingsMenuItem);
  brightnessMenuItem = menu.addMenuItem(brightnessHigh ? "Brightness: HIGH" : "Brightness: LOW", toggleBrightness, settingsMenuItem);
  // About
  aboutMenuItem = menu.addMenuItem("About", showAbout, rootMenu);
  menu.enterSubMenu(rootMenu);
}

// (Legacy) update static settings menu labels
void updateSettingsMenuLabels() {
  settingsMenu[0] = soundOn ? "Sound: ON" : "Sound: OFF";
  settingsMenu[1] = brightnessHigh ? "Brightness: HIGH" : "Brightness: LOW";
}

// Arduino setup: initialize hardware, settings, and menu
void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  dht.begin();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Load settings from EEPROM
  soundOn = EEPROM.read(EEPROM_SOUND_ADDR) == 1;
  brightnessHigh = EEPROM.read(EEPROM_BRIGHT_ADDR) == 1;

  // Apply settings
  menu.setBeepEnabled(soundOn);
  menu.setBrightnessLevel(brightnessHigh ? 255 : 10);

  menu.begin();
  updateSettingsMenuLabels();
  buildDynamicMenu();
}

// Arduino main loop: handle menu and sensor screens
void loop() {
  if (currentScreen == SCREEN_MENU) {
    menu.update();
  } else {
    updateScreen();
    if (digitalRead(BUTTON_BACK) == LOW) {
      delay(200); // Debounce
      menu.enterSubMenu(rootMenu); // Return to main menu
      currentScreen = SCREEN_MENU;
    }
  }
}

// Draw the current sensor or about screen
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


// No longer needed: backToMain, backToSettings, onMainSelect, onSensorsSelect

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

struct MenuItem {
    const char* label;
    void (*action)();
    MenuItem* children;
    MenuItem* next;      // Next sibling
    MenuItem* parent;
};
