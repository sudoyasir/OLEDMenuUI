#include "MenuUI.h"

MenuUI::MenuUI(Adafruit_SSD1306* disp, int up, int down, int select, int buzzer)
  : display(disp), buttonUpPin(up), buttonDownPin(down),
    buttonSelectPin(select), buzzerPin(buzzer),
    selectedItem(0), topItem(0), itemCount(0),
    menuItems(nullptr), callback(nullptr) {}

void MenuUI::begin() {
  pinMode(buttonUpPin, INPUT_PULLUP);
  pinMode(buttonDownPin, INPUT_PULLUP);
  pinMode(buttonSelectPin, INPUT_PULLUP);
  if (buzzerPin >= 0) pinMode(buzzerPin, OUTPUT);

  display->clearDisplay();
  display->display();
}

void MenuUI::setMenuItems(const char** items, int count) {
  menuItems = items;
  itemCount = count;
  selectedItem = 0;
  topItem = 0;
  drawMenu();
}

void MenuUI::setCallback(void (*cb)(int)) {
  callback = cb;
}

void MenuUI::update() {
  if (digitalRead(buttonUpPin) == LOW) {
    beep();
    moveUp();
    delay(200);
  }
  if (digitalRead(buttonDownPin) == LOW) {
    beep();
    moveDown();
    delay(200);
  }
  if (digitalRead(buttonSelectPin) == LOW) {
    beep();
    if (callback) callback(selectedItem);
    delay(200);
  }
}

void MenuUI::moveUp() {
  if (selectedItem > 0) {
    selectedItem--;
    if (selectedItem < topItem) topItem--;
    drawMenu();
  }
}

void MenuUI::moveDown() {
  if (selectedItem < itemCount - 1) {
    selectedItem++;
    if (selectedItem >= topItem + 2) topItem++;  // Only 2 items visible
    drawMenu();
  }
}

void MenuUI::drawMenu() {
  display->clearDisplay();
  display->setTextSize(2);   // Each char ~16px high
  display->setTextColor(SSD1306_WHITE);
  display->setFont();        // Default font (non-bitmap)

  const uint8_t itemHeight = 16;
  const uint8_t paddingY = 1;  // safer vertical offset

  for (uint8_t i = 0; i < 2; ++i) {
    uint8_t itemIndex = topItem + i;
    if (itemIndex >= itemCount) break;

    uint8_t y = i * itemHeight;

    // Draw selector background
    if (itemIndex == selectedItem) {
      display->fillRect(0, y, 128, itemHeight, SSD1306_WHITE);
      display->setTextColor(SSD1306_BLACK);
      display->setCursor(4, y + paddingY);     // padding from left + top
      display->setCursor(16, y + paddingY);    // item text
      display->print(menuItems[itemIndex]);
    } else {
      display->setTextColor(SSD1306_WHITE);
      display->setCursor(16, y + paddingY);
      display->print(menuItems[itemIndex]);
    }
  }

  // Scroll arrows (always visible within bounds)
  if (topItem > 0) {
    display->fillTriangle(122, 1, 125, 4, 128, 1, SSD1306_WHITE);  // ↑
  }
  if (topItem + 2 < itemCount) {
    display->fillTriangle(122, 30, 125, 27, 128, 30, SSD1306_WHITE);  // ↓
  }

  display->display();
}


void MenuUI::beep() {
  if (buzzerPin >= 0) {
    tone(buzzerPin, 1000, 50);  // 1 kHz, 50 ms
    delay(60);
    noTone(buzzerPin);
  }
}
