#include <GUI.hpp>

uint8_t GUI::childCount = 0;
GUI *GUI::children[GUI_MAX_COUNT] = {0};

GUI::GUI(Adafruit_SSD1306 *displayDriver, uint8_t posX, uint8_t posY) : displayDriver(displayDriver), posX(posX), posY(posY) {
    if (childCount >= GUI_MAX_COUNT) {
        delete this;
        return;
    }

    children[childCount] = this;
    childCount++;
}

void GUI::draw() {

}

void GUI::DrawAll() {
    for (uint8_t i = 0; i < childCount; i++) {
        children[i]->draw();
    }
}
