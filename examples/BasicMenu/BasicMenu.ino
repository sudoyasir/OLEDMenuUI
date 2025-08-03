#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "MenuUI.h"

// Display setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pins
const int BUTTON_UP = 2;
const int BUTTON_DOWN = 3;
const int BUTTON_SELECT = 4;
const int BUTTON_BACK = 5;
const int BUZZER = 6;

MenuUI menu(&display, BUTTON_UP, BUTTON_DOWN, BUTTON_SELECT, BUTTON_BACK, BUZZER);

// States
bool soundOn = true;
bool brightnessHigh = true;

// Menus
const char* mainMenu[] = { "Temperature", "Settings", "About" };
const char* settingsMenu[] = { "Toggle Sound", "Brightness", "Back" };

void onMainSelect(int index) {
  if (index == 0) {
    showMessage("Temp: 25C");
  } else if (index == 1) {
    menu.setMenuItems(settingsMenu, 3);
    menu.setCallback(onSettingsSelect);
    menu.setBackCallback(backToMain);
  } else if (index == 2) {
    showMessage("OLED MenuUI v1");
  }
}

void onSettingsSelect(int index) {
  if (index == 0) {
    soundOn = !soundOn;
    menu.setBeepEnabled(soundOn);
    showMessage(soundOn ? "Sound ON" : "Sound OFF");
  } else if (index == 1) {
    brightnessHigh = !brightnessHigh;
    menu.setBrightnessLevel(brightnessHigh ? 255 : 30);
    showMessage(brightnessHigh ? "Bright High" : "Bright Low");
  } else if (index == 2) {
    backToMain();
  }
}

void backToMain() {
  menu.setMenuItems(mainMenu, 3);
  menu.setCallback(onMainSelect);
  menu.setBackCallback(nullptr);  // No back from main menu
}

void showMessage(const char* msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(msg);
  display.display();
  menu.setBackCallback(backToMain);

}

void setup() {
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Display failed");
    while (1);
  }

  menu.begin();
  menu.setMenuItems(mainMenu, 3);
  menu.setCallback(onMainSelect);
  menu.setBeepEnabled(true);
  menu.setBrightnessLevel(255);
}

void loop() {
  menu.update();
}
