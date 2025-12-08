#ifndef ICON_HPP
#define ICON_HPP

#include <GUI.hpp>
#include <Sprites.hpp>

#define ICON_SIZE 16

class Icon : GUI {

    uint8_t const *icon_bitmap = sprite_icons[0];

    void draw() override;

    public:
        Icon(Adafruit_SSD1306 *displayDriver, uint8_t posX, uint8_t posY);
        void setIcon(uint8_t icon);

    static uint8_t FaceToIcon(uint8_t face);
};

#endif