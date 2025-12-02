#include <Bar.hpp>

Bar::Bar(Adafruit_SSD1306 *displayDriver, uint8_t posX, uint8_t posY, uint8_t height, uint8_t width, uint8_t maxCapacity, uint8_t currentCapacity)
: GUI(displayDriver, posX, posY), height(height), width(width), maxCapacity(maxCapacity), currentCapacity(currentCapacity) {

}

void Bar::draw() {
    displayDriver->fillRect(0, 0, 20, 100, SSD1306_INVERSE);
    // uint16_t contentWidth = currentCapacity * width / maxCapacity;
    // displayDriver->fillRect(posX + 1, posY + 1, contentWidth, height - 1, SSD1306_INVERSE);
}

void Bar::setCapacity(uint8_t newCapacity) {
    if (newCapacity > maxCapacity) {
        currentCapacity = maxCapacity;
        return;
    }

    currentCapacity = newCapacity;
}