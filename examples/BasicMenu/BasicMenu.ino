#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <EEPROM.h>
#include <MenuUI.h>

// OLED config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pins
const int BUTTON_UP     = 2;
const int BUTTON_DOWN   = 3;
const int BUTTON_SELECT = 4;
const int BUTTON_BACK   = 5;
const int BUZZER_PIN    = 6;

// EEPROM addresses
#define EEPROM_SOUND_ADDR     0
#define EEPROM_BRIGHTNESS_ADDR 1

// State flags
bool soundEnabled = true;
bool brightnessHigh = true;

// Menu arrays
const char* mainMenu[] = {
  "Temperature",
  "Settings",
  "About"
};

char soundLabel[20];
char brightnessLabel[20];
const char* settingsMenu[3];  // Will point to the above two + "Back"

MenuUI menu(&display, BUTTON_UP, BUTTON_DOWN, BUTTON_SELECT, BUTTON_BACK, BUZZER_PIN);

// ======= Function declarations =======
void onMainSelect(int index);
void onSettingsSelect(int index);
void backToMain();
void showMessage(const char* msg);
void refreshSettingsMenu();
void applyBrightness();
void loadSettings();
void saveSettings();

// ======= Setup =======
void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed");
    while (1);
  }

  loadSettings();
  applyBrightness();

  menu.begin();
  menu.setBeepEnabled(soundEnabled);
  menu.setMenuItems(mainMenu, 3);
  menu.setCallback(onMainSelect);
}

// ======= Main Loop =======
void loop() {
  menu.update();
}

// ======= Callbacks =======
void onMainSelect(int index) {
  switch (index) {
    case 0:
      showMessage("Temp: 25C");
      break;
    case 1:
      refreshSettingsMenu();
      menu.setMenuItems(settingsMenu, 3);
      menu.setCallback(onSettingsSelect);
      menu.setBackCallback(backToMain);
      break;
    case 2:
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("OLED MenuUI v1");
      display.setCursor(0, 16);
      display.println("@sudoyasir"); //You can change or remove it
      display.display();

      while (digitalRead(BUTTON_BACK) == HIGH);  // Wait for back press
      delay(200);
      backToMain();
      break;
  }
}

void onSettingsSelect(int index) {
  switch (index) {
    case 0:
      soundEnabled = !soundEnabled;
      menu.setBeepEnabled(soundEnabled);
      saveSettings();
      break;
    case 1:
      brightnessHigh = !brightnessHigh;
      applyBrightness();
      saveSettings();
      break;
    case 2:
      backToMain();
      return;
  }
  refreshSettingsMenu();  // Refresh text to reflect updated state
  menu.setMenuItems(settingsMenu, 3);
}

// ======= UI Helpers =======

void backToMain() {
  menu.setMenuItems(mainMenu, 3);
  menu.setCallback(onMainSelect);
  menu.setBackCallback(nullptr);
}

void showMessage(const char* msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(msg);
  display.display();

  while (digitalRead(BUTTON_BACK) == HIGH);  // Wait for back press
  delay(200);
  backToMain();
}

void refreshSettingsMenu() {
  snprintf(soundLabel, sizeof(soundLabel), "Sound: %s", soundEnabled ? "ON" : "OFF");
  snprintf(brightnessLabel, sizeof(brightnessLabel), "Brightness: %s", brightnessHigh ? "HIGH" : "LOW");

  settingsMenu[0] = soundLabel;
  settingsMenu[1] = brightnessLabel;
  settingsMenu[2] = "Back";
}

// ======= Settings Handling =======

void loadSettings() {
  soundEnabled = EEPROM.read(EEPROM_SOUND_ADDR) == 1;
  brightnessHigh = EEPROM.read(EEPROM_BRIGHTNESS_ADDR) == 1;
}

void saveSettings() {
  EEPROM.write(EEPROM_SOUND_ADDR, soundEnabled ? 1 : 0);
  EEPROM.write(EEPROM_BRIGHTNESS_ADDR, brightnessHigh ? 1 : 0);
}

void applyBrightness() {
  uint8_t contrast = brightnessHigh ? 255 : 50;
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(contrast);
}
