#include <Pet.hpp>

Pet::Pet(Adafruit_SSD1306 *display) : display_driver(display) {
    body = sprite_bodies[random(0, BODY_COUNT)];
    head = sprite_heads[random(0, HEAD_COUNT)];
    x = display_driver->width() / 2;
    y = display_driver->height() / 2;
}

void Pet::draw() {
    // Head
    display_driver->drawBitmap(x - SPRITE_WIDTH / 2, y - SPRITE_HEIGHT / 2, head, HEAD_WIDTH, HEAD_HEIGHT, SSD1306_INVERSE);
    display_driver->drawBitmap(x - SPRITE_WIDTH / 2, y, body, BODY_WIDTH, BODY_HEIGHT, SSD1306_INVERSE);
}