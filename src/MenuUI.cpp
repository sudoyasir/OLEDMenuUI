#include "MenuUI.h"
#include <Arduino.h>

MenuUI::MenuUI(Adafruit_SSD1306* disp, int upPin, int downPin, int selectPin, int buzzPin) {
  display = disp;
  buttonUp = upPin;
  buttonDown = downPin;
  buttonSelect = selectPin;
  buzzerPin = buzzPin;
  menuItems = nullptr;
  menuItemCount = 0;
  currentIndex = 0;
  callback = nullptr;
}

void MenuUI::begin() {
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonSelect, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
}

void MenuUI::setMenuItems(const char** items, int count) {
  menuItems = items;
  menuItemCount = count;
  currentIndex = 0;
  drawMenu();
}

void MenuUI::setCallback(void (*cb)(int)) {
  callback = cb;
}

void MenuUI::beep(int freq, int dur) {
  tone(buzzerPin, freq, dur);
}

void MenuUI::update() {
  if (digitalRead(buttonUp) == LOW) {
    currentIndex = (currentIndex - 1 + menuItemCount) % menuItemCount;
    beep(1000, 80);
    drawMenu();
    delay(200);
  }

  if (digitalRead(buttonDown) == LOW) {
    currentIndex = (currentIndex + 1) % menuItemCount;
    beep(1000, 80);
    drawMenu();
    delay(200);
  }

  if (digitalRead(buttonSelect) == LOW) {
    beep(1500, 100);
    if (callback) callback(currentIndex);
    delay(300);
  }
}

void MenuUI::drawMenu() {
  display->clearDisplay();
  display->setTextSize(1);  // Use default 6x8 font
  display->setTextColor(SSD1306_WHITE);

  const int itemsPerPage = 2;
  const int itemHeight = 12;
  const int screenHeight = 32;

  int startIdx = (currentIndex / itemsPerPage) * itemsPerPage;

  for (int i = 0; i < itemsPerPage; i++) {
    int itemIndex = startIdx + i;
    int y = i * itemHeight;

    if (itemIndex >= menuItemCount) break;

    if (itemIndex == currentIndex) {
      // Draw highlight rectangle
      display->fillRect(0, y, 128, itemHeight, SSD1306_WHITE);
      display->setTextColor(SSD1306_BLACK); // Invert text
    } else {
      display->setTextColor(SSD1306_WHITE);
    }

    display->setCursor(2, y + 2);
    display->println(menuItems[itemIndex]);
  }

  // Scroll arrows
  if (startIdx + itemsPerPage < menuItemCount) {
    display->setCursor(118, screenHeight - 10);
    display->print("↓");
  }

  if (startIdx > 0) {
    display->setCursor(118, 0);
    display->print("↑");
  }

  display->display();
}
