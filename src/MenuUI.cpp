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
  display->setTextSize(2);   // 16-pixel height
  display->setTextColor(SSD1306_WHITE);
  display->setFont();        // Use default font

  for (uint8_t i = 0; i < 2; ++i) {
    uint8_t itemIndex = topItem + i;
    if (itemIndex >= itemCount) break;

    uint8_t y = i * 16;  // 2 lines, 16 pixels each (2 * 16 = 32)

    if (itemIndex == selectedItem) {
      display->fillRect(0, y, 128, 16, SSD1306_WHITE);
      display->setTextColor(SSD1306_BLACK);
    } else {
      display->setTextColor(SSD1306_WHITE);
    }

    display->setCursor(4, y + 1);  // small padding from left/top
    display->print(menuItems[itemIndex]);
  }

  // Scroll arrows (optional)
  if (topItem > 0) {
    display->fillTriangle(124, 2, 127, 2, 125, 0, SSD1306_WHITE);  // up arrow
  }
  if (topItem + 2 < itemCount) {
    display->fillTriangle(124, 30, 127, 30, 125, 32, SSD1306_WHITE);  // down arrow
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
