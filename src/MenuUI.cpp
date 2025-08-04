#include "MenuUI.h"

MenuUI::MenuUI(Adafruit_SSD1306 *disp, int up, int down, int select, int back, int buzzer)
    : display(disp), buttonUpPin(up), buttonDownPin(down), buttonSelectPin(select),
      buttonBackPin(back), buzzerPin(buzzer), selectedItem(0), topItem(0), itemCount(0),
      menuItems(nullptr), callback(nullptr), backCallback(nullptr), beepEnabled(true) {}

void MenuUI::begin()
{
  pinMode(buttonUpPin, INPUT_PULLUP);
  pinMode(buttonDownPin, INPUT_PULLUP);
  pinMode(buttonSelectPin, INPUT_PULLUP);
  pinMode(buttonBackPin, INPUT_PULLUP);
  if (buzzerPin >= 0)
    pinMode(buzzerPin, OUTPUT);

  display->clearDisplay();
  display->display();
}

void MenuUI::setMenuItems(const char **items, int count, bool resetSelection)
{
  menuItems = items;
  itemCount = count;
  if (resetSelection)
  {
    selectedItem = 0;
    topItem = 0;
  }
  drawMenu();
}

void MenuUI::setCallback(void (*cb)(int))
{
  callback = cb;
}

void MenuUI::setBackCallback(void (*cb)())
{
  backCallback = cb;
}

void MenuUI::setBeepEnabled(bool enabled)
{
  beepEnabled = enabled;
}

void MenuUI::setBrightnessLevel(uint8_t level)
{
  display->ssd1306_command(SSD1306_SETCONTRAST);
  display->ssd1306_command(level);
}

void MenuUI::update()
{
  if (digitalRead(buttonUpPin) == LOW)
  {
    beep();
    moveUp();
    delay(200);
  }
  if (digitalRead(buttonDownPin) == LOW)
  {
    beep();
    moveDown();
    delay(200);
  }
  if (digitalRead(buttonSelectPin) == LOW)
  {
    beep(1);
    if (callback)
      callback(selectedItem);
    delay(200);
  }
  if (digitalRead(buttonBackPin) == LOW)
  {
    beep();
    if (backCallback)
      backCallback();
    delay(200);
  }
}

void MenuUI::moveUp()
{
  if (selectedItem > 0)
  {
    selectedItem--;
    if (selectedItem < topItem)
      topItem--;
    drawMenu();
  }
}

void MenuUI::moveDown()
{
  if (selectedItem < itemCount - 1)
  {
    selectedItem++;
    if (selectedItem >= topItem + maxVisibleItems)
      topItem++;
    drawMenu();
  }
}

void MenuUI::drawMenu()
{
  display->clearDisplay();
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);

  int paddingX = 6;
  int paddingY = 4;
  int selectorHeight = 16;
  int cornerRadius = 4;

  for (int i = 0; i < maxVisibleItems; ++i)
  {
    int index = topItem + i;
    if (index >= itemCount)
      break;

    int y = i * selectorHeight;

    if (index == selectedItem)
    {
      display->fillRoundRect(0, y, display->width(), selectorHeight, cornerRadius, SSD1306_WHITE);
      display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    }
    else
    {
      display->setTextColor(SSD1306_WHITE);
    }

    display->setCursor(paddingX, y + paddingY);
    display->print(menuItems[index]);
  }

  // Arrows
  if (topItem > 0)
    display->fillTriangle(120, 2, 125, 2, 122, 0, SSD1306_WHITE);
  if (topItem + maxVisibleItems < itemCount)
    display->fillTriangle(120, 30, 125, 30, 122, 32, SSD1306_WHITE);

  display->display();
}

void MenuUI::beep(int type)
{
  if (!beepEnabled || buzzerPin < 0)
    return;

  if (type == 1)
  {
    tone(buzzerPin, 1500, 100);
  }
  else
  {
    tone(buzzerPin, 1000, 50);
  }
  delay(60);
  noTone(buzzerPin);
}
