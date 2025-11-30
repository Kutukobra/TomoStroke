#ifndef PET_HPP
#define PET_HPP

#include <Arduino.h>
#include <Sprites.hpp>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define TONE_MIN 500
#define TONE_MAX 1500

class Pet {
    Adafruit_SSD1306 *display_driver;
    uint8_t const *body, *head;

    uint8_t x, y;

    public:
        Pet(Adafruit_SSD1306 *display);
        void draw();
};

#endif