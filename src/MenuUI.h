#ifndef MENU_UI_H
#define MENU_UI_H

#include <Adafruit_SSD1306.h>

class MenuUI {
  public:
    MenuUI(Adafruit_SSD1306* display, int upPin, int downPin, int selectPin, int buzzerPin);
    void begin();
    void setMenuItems(const char** items, int count);
    void setCallback(void (*callback)(int));
    void update();

  private:
    Adafruit_SSD1306* display;
    const char** menuItems;
    int menuItemCount;
    int currentIndex;
    int buttonUp, buttonDown, buttonSelect;
    int buzzerPin;
    void (*callback)(int);
    void drawMenu();
    void beep(int freq = 1000, int dur = 100);
};

#endif
