# OLEDMenuUI

A lightweight, customizable menu system for Arduino projects using SSD1306 OLED displays. Perfect for building user interfaces on microcontrollers with limited buttons and screen real estate.

## ✨ Features

* Supports 128x32 OLED displays (Adafruit SSD1306)
* Crisp font rendering using `FreeSansBold9pt7b`
* Shows 2 menu items per screen with scroll indicators
* Highlighted the selected item with a rounded rectangle
* Buzzer feedback support on navigation and selection
* Easy to extend: works with 3 buttons (Up, Down, Select)

## 📦 Installation

1. Clone or download this repository.
2. Move the `MenuUI` folder into your Arduino `libraries` directory.
3. Include it in your sketch:

```cpp
#include <MenuUI.h>
```

## 🧰 Usage Example

```cpp
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MenuUI.h>

Adafruit_SSD1306 display(128, 32, &Wire, -1);

const char* menuItems[] = {"Item 1", "Item 2", "Item 3", "Item 4"};

MenuUI menu(&display, 2, 3, 4, 5);  // up, down, select, buzzer pins

void onMenuSelect(int index) {
  // Do something with index
}

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  menu.begin();
  menu.setMenuItems(menuItems, 4);
  menu.setCallback(onMenuSelect);
}

void loop() {
  menu.update();
}
```

## 🔧 Configuration

You can customize:

* Font size and style
* Padding and spacing
* Scroll arrow placement
* Buzzer behavior (optional)

## ✅ To-Do / Issues

* [x] Fix strange horizontal line on screen
* [ ] Add back button support
* [ ] Port to other microcontrollers (ESP32, ESP8266, etc.)
* [ ] Improve font auto-scaling for different screen sizes

## 📁 Examples

This library comes with built-in examples.

- `BasicMenu`: A minimal OLED menu with scrolling, selection, and back functionality.


## 📚 Dependencies

* [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
* [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library)

## 💡 Tips

* Ideal for simple interfaces like settings menus or selecting options
* Works great for battery-powered devices with small displays

## 📜 License

MIT License – feel free to use and modify.
