#ifndef PET_HPP
#define PET_HPP

#include <Arduino.h>
#include <Sprites.hpp>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

typedef struct Vector2D {
    int8_t x, y;
} Vector2D;

#define TONE_MIN 500
#define TONE_MAX 1500

class Pet {
    Adafruit_SSD1306 *display_driver;
    uint8_t body, head, face = FACE_IDLE; // Look index

    Vector2D position, velocity = {0, 0};

    uint64_t blinkLast = 0, blinkInterval;
    bool eyeClosed = false;

    void _blinkCheck();

    public:
        Pet(Adafruit_SSD1306 *display);

        void update();
        void draw();

        void setPosition(int8_t x, int8_t y);
        Vector2D getPosition();

        void jump();
};

#endif