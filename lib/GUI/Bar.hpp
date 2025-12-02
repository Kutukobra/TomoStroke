#ifndef BAR_HPP
#define BAR_HPP

#include <GUI.hpp>

class Bar : GUI {
    uint8_t height, width;
    uint16_t maxCapacity, currentCapacity;

    void draw() override;

    public:
        Bar(Adafruit_SSD1306 *displayDriver, uint8_t posX, uint8_t posY, uint8_t width, uint8_t height, uint16_t currentCapacity, uint16_t maxCapacity);

        void setCapacity(uint16_t newCapacity);
};

#endif