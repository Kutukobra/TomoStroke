#include <Bar.hpp>

Bar::Bar(Adafruit_SSD1306 *displayDriver, uint8_t posX, uint8_t posY, uint8_t width, uint8_t height, uint16_t currentCapacity, uint16_t maxCapacity)
: GUI(displayDriver, posX, posY), width(width), height(height),  currentCapacity(currentCapacity), maxCapacity(maxCapacity) {
    Serial.printf("New Bar\n\t- Position: %d %d\n\t- Dimensions: %d %d\n\t- Capacities %d/%d\n", 
        this->posX, this->posY, this->width, this->height, this->currentCapacity, this->maxCapacity
    );
}

void Bar::draw() {
    displayDriver->drawRect(posX, posY, width, height, SSD1306_INVERSE);
    uint16_t contentWidth = (float)currentCapacity / maxCapacity * width;
    displayDriver->fillRect(posX + 2, posY + 2, contentWidth - 4, height - 4, SSD1306_INVERSE);
}

void Bar::setCapacity(uint16_t newCapacity) {
    if (newCapacity > maxCapacity) {
        currentCapacity = maxCapacity;
        return;
    }

    currentCapacity = newCapacity;
}