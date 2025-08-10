#include "MenuUI.h"
/*
 * MenuUI.cpp - OLED Menu UI Library for Arduino
 * Author: Yasir N. | https://github.com/sudoyasir/OLEDMenuUI
 *
 * MIT License
 *
 * This file implements the MenuUI class, providing both static and dynamic menu support
 * for Arduino projects using SSD1306 OLED displays. The dynamic menu system uses C-style
 * linked lists and function pointers for maximum compatibility with AVR-based boards.
 *
 * Features:
 *   - Dynamic creation and deletion of menu items and submenus at runtime
 *   - Nested submenus with unlimited depth (limited by memory)
 *   - Static array-based menu support for legacy code
 *   - Simple API for menu navigation and action callbacks
 *   - Optional beep and brightness control
 */
      rootMenu(nullptr), currentMenu(nullptr) {}

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
// -----------------------------------------------------------------------------
// Legacy static array support (for backward compatibility)
// -----------------------------------------------------------------------------
  menuItems = items;
  itemCount = count;
  rootMenu = nullptr;
  currentMenu = nullptr;
  if (resetSelection)
  {
    selectedItem = 0;
    topItem = 0;
  }
  drawMenuStatic();
}

// -----------------------------------------------------------------------------
// Dynamic menu API implementation
// -----------------------------------------------------------------------------
MenuUI::MenuItem* MenuUI::addMenuItem(const char* label, void (*action)(), MenuItem* parent) {
    MenuItem* item = new MenuItem;
    item->label = label;
    item->action = action;
    item->children = nullptr;
    item->next = nullptr;
    item->parent = parent;
    if (!parent) {
        // If no parent, this is the root menu
        rootMenu = item;
        currentMenu = rootMenu;
    } else {
        // Add as last child of the parent menu
        if (!parent->children) {
            parent->children = item;
        } else {
            MenuItem* sibling = parent->children;
            while (sibling->next) sibling = sibling->next;
            sibling->next = item;
        }
    }
    return item;
}

void MenuUI::removeMenuItem(MenuItem* item) {
    if (!item || !item->parent) return;
    MenuItem* parent = item->parent;
    MenuItem* prev = nullptr;
    MenuItem* cur = parent->children;
    while (cur) {
        if (cur == item) {
            if (prev) prev->next = cur->next;
            else parent->children = cur->next;
            // Recursively delete all children of this menu item
            MenuItem* child = cur->children;
            while (child) {
                MenuItem* nextChild = child->next;
                removeMenuItem(child);
                child = nextChild;
            }
            delete cur;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
}

void MenuUI::clearMenu(MenuItem* parent) {
    if (!parent) parent = rootMenu;
    if (!parent) return;
    MenuItem* child = parent->children;
    while (child) {
        MenuItem* nextChild = child->next;
        removeMenuItem(child);
        child = nextChild;
    }
    parent->children = nullptr;
    if (parent == rootMenu) {
        selectedItem = 0;
        topItem = 0;
        currentMenu = rootMenu;
    }
}

void MenuUI::enterSubMenu(MenuItem* submenu) {
    if (!submenu || !submenu->children) return;
    currentMenu = submenu;
    selectedItem = 0;
    topItem = 0;
    drawMenuDynamic();
}

void MenuUI::backToParent() {
    if (currentMenu && currentMenu->parent) {
        currentMenu = currentMenu->parent;
        selectedItem = 0;
        topItem = 0;
        drawMenuDynamic();
    }
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
  if (rootMenu && currentMenu) {
    // Dynamic menu mode: traverse linked list structure
    int itemCount = 0;
    MenuItem* child = currentMenu->children;
    while (child) {
      itemCount++;
      child = child->next;
    }
    if (digitalRead(buttonUpPin) == LOW) {
      beep();
      if (selectedItem > 0) {
        selectedItem--;
        if (selectedItem < topItem) topItem--;
        drawMenuDynamic();
      }
      delay(200);
    }
    if (digitalRead(buttonDownPin) == LOW) {
      beep();
      if (selectedItem < itemCount - 1) {
        selectedItem++;
        if (selectedItem >= topItem + maxVisibleItems) topItem++;
        drawMenuDynamic();
      }
      delay(200);
    }
    if (digitalRead(buttonSelectPin) == LOW) {
      beep(1);
      if (itemCount > 0) {
        int idx = 0;
        MenuItem* sel = currentMenu->children;
        while (sel && idx < selectedItem) { sel = sel->next; idx++; }
        if (sel) {
          if (sel->children) {
            enterSubMenu(sel);
          } else if (sel->action) {
            sel->action();
          }
        }
      }
      delay(200);
    }
    if (digitalRead(buttonBackPin) == LOW) {
      beep();
      backToParent();
      delay(200);
    }
  } else {
    // Legacy static menu mode: use static array
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


void MenuUI::drawMenu() {
  if (rootMenu && currentMenu) drawMenuDynamic();
  else drawMenuStatic();
}

void MenuUI::drawMenuStatic() {
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

  // Draw up/down arrows for menu navigation
  if (topItem > 0)
    display->fillTriangle(120, 2, 125, 2, 122, 0, SSD1306_WHITE);
  if (topItem + maxVisibleItems < itemCount)
    display->fillTriangle(120, 30, 125, 30, 122, 32, SSD1306_WHITE);

  display->display();
}


void MenuUI::drawMenuDynamic() {
  display->clearDisplay();
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);

  int paddingX = 6;
  int paddingY = 4;
  int selectorHeight = 16;
  int cornerRadius = 4;
  int itemCount = 0;
  MenuItem* child = currentMenu->children;
  while (child) {
    itemCount++;
    child = child->next;
  }

  for (int i = 0; i < maxVisibleItems; ++i)
  {
    int index = topItem + i;
    if (index >= itemCount)
      break;

    int y = i * selectorHeight;
    int idx = 0;
    MenuItem* item = currentMenu->children;
    while (item && idx < index) { item = item->next; idx++; }
    if (!item) break;

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
    display->print(item->label);
    if (item->children) {
      display->setCursor(display->width() - 12, y + paddingY);
      display->print(">>");
    }
  }

  // Draw up/down arrows for menu navigation
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
