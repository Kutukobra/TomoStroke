#include <Pet.hpp>

Pet::Pet(Adafruit_SSD1306 *display) : display_driver(display) {
    body = sprite_bodies[random(0, BODY_COUNT)];
    head = sprite_heads[random(0, HEAD_COUNT)];
    x = display_driver->width() / 2;
    y = display_driver->height() / 2;
    face_frame = FACE_IDLE;
    last_blink = millis();
    blinking = false;
    idle_frame = 0;
    last_idle_anim = millis();
}

void Pet::update() {
    unsigned long now = millis();

    // Idle animation: switch frame every 400ms
    if (now - last_idle_anim > 400) {
        idle_frame = (idle_frame + 1) % IDLE_FRAME_COUNT;
        last_idle_anim = now;
    }

    // Eye blink logic (as before)
    if (!blinking && now - last_blink > 2000) {
        face_frame = FACE_BLINK;
        blinking = true;
        last_blink = now;
    } else if (blinking && now - last_blink > 200) {
        face_frame = FACE_IDLE;
        blinking = false;
        last_blink = now;
    }
}

void Pet::draw() {
    // Head
    display_driver->drawBitmap(x - SPRITE_WIDTH / 2, y - SPRITE_HEIGHT / 2, head, HEAD_WIDTH, HEAD_HEIGHT, SSD1306_INVERSE);
    // Face
    display_driver->drawBitmap(x - FACE_WIDTH / 2, y - FACE_HEIGHT / 2, sprite_faces[face_frame], FACE_WIDTH, FACE_HEIGHT, SSD1306_INVERSE);
    // Animated body
    display_driver->drawBitmap(x - SPRITE_WIDTH / 2, y, sprite_bodies_idle[idle_frame], BODY_WIDTH, BODY_HEIGHT, SSD1306_INVERSE);
}