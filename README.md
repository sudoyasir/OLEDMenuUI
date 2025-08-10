# OLEDMenuUI

**OLEDMenuUI** is an Arduino library for creating interactive, scrollable menus on SSD1306 OLED displays using physical buttons. It now supports **dynamic (runtime) menu creation and deletion**, unlimited nested submenus, sensor data display, EEPROM-persistent settings, and UI features like rounded selection indicators. The library is fully compatible with Arduino Uno and other AVR boards (no STL required).

✨ Features

- ✅ **Dynamic (runtime) menu and submenu creation**
- ✅ Unlimited nested submenus (C-style linked lists, no STL)
- ✅ Add/remove menu items at runtime
- ✅ Scrollable menus with arrow navigation
- ✅ Support for Up, Down, Select, and Back buttons
- ✅ EEPROM-persistent **Sound** and **Brightness** settings
- ✅ Rounded corner selector with left-padding
- ✅ Easy integration of sensor displays (e.g., DHT11, HC-SR04)
- ✅ Modular design with callbacks
- ✅ Designed for SSD1306 (128x32) displays

## ✅ Supported Boards

| Board         | OLED Support | EEPROM Support | Tested | Notes |
|---------------|--------------|----------------|--------|-------|
| Arduino Uno   | ✅            | ✅              | ✅      | Default test board |
| Arduino Nano  | ✅            | ✅              | ✅      | Same pinout as Uno |
| Arduino Mega  | ✅            | ✅              | ✅      | Use pins 20 (SDA), 21 (SCL) |
| ESP8266 (NodeMCU) | ✅       | ⚠️ Partial      | ✅      | Use `EEPROM.begin()` and `EEPROM.commit()` |
| ESP32         | ✅            | ✅              | ✅      | Use pins 21 (SDA), 22 (SCL) by default |
| Arduino Leonardo | ✅        | ✅              | 🚫      | Not tested yet |

## 🧱 Requirements

- Arduino-compatible board
- SSD1306 128x32 I2C OLED display
- 4 push buttons (Up, Down, Select, Back)
- Optional: DHT11 Sensor, HC-SR04 Sensor
- Optional: Buzzer for sound feedback


## 🔧 Installation

Clone the repository into your `Arduino/libraries` directory:

```bash
git clone https://github.com/sudoyasir/OLEDMenuUI.git
````

Or use the **Add .ZIP Library** option in Arduino IDE.


## 🧩 Library Structure

```text
OLEDMenuUI/
├── examples/
│   └── SensorMenu/        # Full demo with sensors & settings
├── src/
│   └── MenuUI.h/.cpp      # Core menu library
├── library.properties
└── README.md
```

## 📦 EEPROM Settings

The menu supports saving **Sound** and **Brightness** settings to EEPROM.

| Setting    | EEPROM Address | Values         |
| ---------- | -------------- | -------------- |
| Sound      | `0`            | `ON` / `OFF`   |
| Brightness | `1`            | `HIGH` / `LOW` |

## 🎮 Button Configuration

```cpp
#define BUTTON_UP     2
#define BUTTON_DOWN   3
#define BUTTON_SELECT 4
#define BUTTON_BACK   5
#define BUZZER_PIN    6
```

## 📊 Sensor Support (Example)

### DHT11 Temperature & Humidity

```cpp
#include <DHT.h>
#define DHTPIN 7
DHT dht(DHTPIN, DHT11);
```

### HC-SR04 Distance

```cpp
#define TRIG_PIN 8
#define ECHO_PIN 9
```


## 📁 Example Usage (Dynamic Menu API)

A full example is included in `examples/SensorMenu/SensorMenu.ino`.

### Dynamic Menu Creation Example

```cpp
// Build the dynamic menu structure at runtime
void buildDynamicMenu() {
  menu.clearMenu();
  MenuUI::MenuItem* rootMenu = menu.addMenuItem("ROOT");
  // Sensors submenu
  MenuUI::MenuItem* sensorsMenu = menu.addMenuItem("Sensors", nullptr, rootMenu);
  menu.addMenuItem("Temperature", showTemp, sensorsMenu);
  menu.addMenuItem("Humidity", showHumid, sensorsMenu);
  menu.addMenuItem("Distance", showDist, sensorsMenu);
  // Settings submenu
  MenuUI::MenuItem* settingsMenu = menu.addMenuItem("Settings", nullptr, rootMenu);
  menu.addMenuItem(soundOn ? "Sound: ON" : "Sound: OFF", toggleSound, settingsMenu);
  menu.addMenuItem(brightnessHigh ? "Brightness: HIGH" : "Brightness: LOW", toggleBrightness, settingsMenu);
  // About
  menu.addMenuItem("About", showAbout, rootMenu);
  menu.enterSubMenu(rootMenu);
}
```

### Main Menu Items

- Sensors (submenu)
  - Temperature (from DHT11)
  - Humidity (from DHT11)
  - Distance (from HC-SR04)
- Settings (submenu)
  - Sound: ON / OFF
  - Brightness: HIGH / LOW
- About

### Dynamic Label Update Example

```cpp
void toggleSound() {
  soundOn = !soundOn;
  EEPROM.write(EEPROM_SOUND_ADDR, soundOn ? 1 : 0);
  menu.setBeepEnabled(soundOn);
  soundMenuItem->label = soundOn ? "Sound: ON" : "Sound: OFF";
  menu.drawMenu();
}
```

## ⚙️ Customization & Professional Code

You can customize:

- Fonts (`setTextSize`, `setCursor`)
- Selector style (rounded corners, padding)
- Sound feedback (buzzer or silent)
- Brightness control (adjust contrast level)

All example and library code is now professionally commented for clarity and maintainability.

## 🛠️ Arduino Uno & AVR Compatibility

The dynamic menu system uses C-style linked lists and function pointers for full compatibility with Arduino Uno and other AVR boards. No STL or dynamic memory allocation is required.

## 📜 License

MIT License © 2025 [Yasir Nawaz](https://github.com/sudoyasir)

## 🙌 Acknowledgments

* [Adafruit SSD1306 Library](https://github.com/adafruit/Adafruit_SSD1306)
* [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
* [Arduino EEPROM](https://www.arduino.cc/en/Reference/EEPROM)

> [!TIP]
> Want to contribute or request features? Open an issue or submit a PR. Contributions welcome!


Let me know if you want:
- A GIF or screenshot badge included
- A version badge or install badge
- Help writing `library.properties`

And I can tailor it more if you want this to be Arduino Library Manager–friendly.
