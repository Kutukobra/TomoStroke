#include <Arduino.h>
#include <Sprites.hpp>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Pet {
    Adafruit_SSD1306 *display;
    uint8_t *face, *head;

    public:
        Pet(Adafruit_SSD1306 *display) : display(display) {
            
        }
};