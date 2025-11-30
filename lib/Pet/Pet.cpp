#include <Pet.hpp>

Pet::Pet(Adafruit_SSD1306 *display) : display_driver(display) {
    body = random(0, BODY_COUNT);
    head = random(0, HEAD_COUNT);
    
    blinkInterval = random(BLINK_MIN, BLINK_MAX);

    speakInterval = random(SPEAK_MIN, SPEAK_MAX);

    position = {64, 32};

    Serial.printf("New Pet. \n\tBody: %d\n\tFace: %d\n\tBlink Interval: %d\n\tSpeak Interval: %d\n", body, face, blinkInterval, speakInterval);
}

void Pet::update() {

    _blinkCheck();
    _speakCheck();

    // Position
    velocity.y += GRAVITY;

    position.x += velocity.x;
    position.y += velocity.y;

    if (position.y - SPRITE_HEIGHT / 2 <= 16) { // Limit top bar
        position.y = SPRITE_HEIGHT / 2 + 16;
    }
    
    if (position.y + SPRITE_HEIGHT / 2 >= 64) {
        velocity.y = 0;
        position.y = 63 - SPRITE_HEIGHT / 2;
    }
}

void Pet::_blinkCheck() {
    uint64_t currentTime = millis();

    if (eyeClosed) {
        if (currentTime - blinkLast >= BLINK_CLOSED_DEFAULT + blinkInterval / 10) {
            eyeClosed = false;
            blinkLast = currentTime;
        }
    } else {
        if (currentTime - blinkLast >= blinkInterval) {
            eyeClosed = true;
            blinkLast = currentTime;
        }
    }
}

void Pet::_speakCheck() {
    uint64_t currentTime = millis();

    if (currentTime - speakLast >= speakInterval) {
        Serial.println("Pet Spoke.");
        speakLast = currentTime + random(-SPEAK_INTERVAL_OFFSET, SPEAK_INTERVAL_OFFSET);
    }
}

void Pet::draw() {

    uint8_t drawFace = eyeClosed ? FACE_BLINK : face;

    // Head
    display_driver->drawBitmap(position.x - SPRITE_WIDTH / 2, position.y - SPRITE_HEIGHT / 2, sprite_heads[head], HEAD_WIDTH, HEAD_HEIGHT, SSD1306_INVERSE);
    // Body
    display_driver->drawBitmap(position.x - SPRITE_WIDTH / 2, position.y, sprite_bodies[body], BODY_WIDTH, BODY_HEIGHT, SSD1306_INVERSE);
    // Face
    display_driver->drawBitmap(position.x - SPRITE_WIDTH / 4, position.y - SPRITE_HEIGHT / 4, sprite_faces[drawFace], FACE_WIDTH, FACE_HEIGHT, SSD1306_INVERSE);

    if (isHighlighted) {
        display_driver->drawRect(position.x - SPRITE_WIDTH / 2, position.y - SPRITE_HEIGHT / 2, 32, 32, SSD1306_INVERSE);
    }
}

void Pet::setPosition(int8_t newX, int8_t newY) {
    position = {newX, newY};
}

Vector2D Pet::getPosition() {
    return position;
}

void Pet::jump() {
    velocity.y = -10;
}