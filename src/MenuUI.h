

/*
 * OLEDMenuUI Library
 *
 * A lightweight, Arduino-compatible menu UI library for SSD1306 OLED displays.
 * Supports both static and dynamic (runtime) menu structures, including nested submenus.
 *
 * Author: Yasir N.  |  GitHub: https://github.com/sudoyasir/OLEDMenuUI
 * Portfolio: https://sudoyasir.space
 */

#ifndef MENU_UI_H
#define MENU_UI_H

#include <Adafruit_SSD1306.h>

// MenuUI: Main menu management class
class MenuUI {
public:
    // MenuItem: Represents a single menu entry (can be a submenu or action)
    struct MenuItem {
        const char* label;           // Display label
        void (*action)();           // Action callback (nullptr for submenu)
        MenuItem* children;         // First child (submenu)
        MenuItem* next;             // Next sibling in menu
        MenuItem* parent;           // Parent menu
    };

    // Constructor: Initialize with display and button pins
    MenuUI(Adafruit_SSD1306 *display, int up, int down, int select, int back, int buzzer);
    void begin();

    // Static menu API (backward compatible)
    void setMenuItems(const char *items[], int count, bool resetSelection = true);

    // Dynamic menu API
    MenuItem* addMenuItem(const char* label, void (*action)() = nullptr, MenuItem* parent = nullptr);
    void removeMenuItem(MenuItem* item);
    void clearMenu(MenuItem* parent = nullptr);
    void enterSubMenu(MenuItem* submenu);
    void backToParent();

    // Menu navigation and rendering
    void update();
    void drawMenu();

    // Optional features
    void setCallback(void (*callback)(int));
    void setBackCallback(void (*callback)());
    void setBeepEnabled(bool enabled);
    void setBrightnessLevel(uint8_t level);

private:
    Adafruit_SSD1306 *display;      // OLED display pointer
    const char **menuItems;         // Static menu items
    int itemCount;                  // Number of static items

    MenuItem* rootMenu;             // Root of dynamic menu
    MenuItem* currentMenu;          // Currently displayed menu
    int selectedItem;               // Currently selected item index
    int topItem;                    // Top visible item index
    int maxVisibleItems = 2;        // Max items visible at once

    int buttonUpPin;
    int buttonDownPin;
    int buttonSelectPin;
    int buttonBackPin;
    int buzzerPin;

    bool beepEnabled = true;

    void (*callback)(int);          // Static menu select callback
    void (*backCallback)();         // Static menu back callback

    void moveUp();                  // Move selection up
    void moveDown();                // Move selection down
    void beep(int type = 0);        // Play beep sound
    void drawMenuDynamic();         // Draw dynamic menu
    void drawMenuStatic();          // Draw static menu
};

#endif
