#ifndef MENU_UI_H
#define MENU_UI_H

#include <Adafruit_SSD1306.h>

class MenuUI {
  public:
    MenuUI(Adafruit_SSD1306* display, int up, int down, int select, int buzzer);
    void begin();
    void setMenuItems(const char* items[], int count);
    void update();
    void drawMenu();
    void setCallback(void (*callback)(int));

  private:
    Adafruit_SSD1306* display;
    const char** menuItems;

    int selectedItem;
    int topItem;
    int itemCount;
    int maxVisibleItems = 2;

    int buttonUpPin;
    int buttonDownPin;
    int buttonSelectPin;
    int buzzerPin;

    void (*callback)(int);

    void moveUp();
    void moveDown();
    void beep();
};

#endif
