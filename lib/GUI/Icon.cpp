#include <Icon.hpp>

Icon::Icon(Adafruit_SSD1306 *displayDriver, uint8_t posX, uint8_t posY) : GUI(displayDriver, posX, posY) {

}


void Icon::draw() {
    displayDriver->drawBitmap(posX, posY, icon_bitmap, ICON_SIZE, ICON_SIZE, SSD1306_INVERSE);
}

void Icon::setIcon(uint8_t icon){
    icon_bitmap = sprite_icons[icon];
}