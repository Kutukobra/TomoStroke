#ifndef GUI_HPP
#define GUI_HPP

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define GUI_MAX_COUNT 4

class GUI {

    private:
        static GUI *children[GUI_MAX_COUNT];
        static uint8_t childCount;

    protected:
        Adafruit_SSD1306 *displayDriver;
        uint8_t posX, posY; // Top left corner of gui component
        virtual void draw() = 0;

    public:
        GUI(Adafruit_SSD1306 *displayDriver, uint8_t posX, uint8_t posY);


    static void DrawAll();
};





#endif