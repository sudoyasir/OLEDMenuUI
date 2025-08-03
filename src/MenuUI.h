#ifndef MENU_UI_H
#define MENU_UI_H

#include <Adafruit_SSD1306.h>

class MenuUI {
  public:
    MenuUI(Adafruit_SSD1306* display, int up, int down, int select, int back, int buzzer);
    void begin();
    void setMenuItems(const char* items[], int count);
    void update();
    void drawMenu();
    void setCallback(void (*callback)(int));
    void setBackCallback(void (*callback)());
    void setBeepEnabled(bool enabled);
    void setBrightnessLevel(uint8_t level);

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
    int buttonBackPin;
    int buzzerPin;

    bool beepEnabled = true;

    void (*callback)(int);
    void (*backCallback)();

    void moveUp();
    void moveDown();
    void beep(int type = 0);
};

#endif
