#include <Pet.hpp>

Pet::Pet(Adafruit_SSD1306 *display, QueueHandle_t voiceMessageQueue) : displayDriver(display), voiceQueue(voiceMessageQueue) {
    body = random(0, BODY_COUNT);
    head = random(0, HEAD_COUNT);
    blinkInterval = random(BLINK_MIN, BLINK_MAX);

    speakInterval = random(SPEAK_MIN, SPEAK_MAX);

    position = {64, 32};

    _generateVoice();

    Serial.printf("- New Pet\n\tBody: %d\n\tFace: %d\n\tBlink Interval: %d\n\n", body, face, blinkInterval);
    Serial.printf("\tVoice (Interval=%d):\n", speakInterval);
    for (uint8_t i = 0; i < voiceLength * 2; i += 2) {
        Serial.printf("\t\tF: %d, D: %d\n", voice[i], voice[i + 1]);
    }
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
        _speakVoice();
        speakLast = currentTime + random(-SPEAK_INTERVAL_OFFSET, SPEAK_INTERVAL_OFFSET);
    }
}

void Pet::_speakVoice() {
    VoiceMessage message;
    message.voiceLength = voiceLength;
    message.voice = voice;
    xQueueSend(voiceQueue, &message, 10);
    Serial.println("Pet Spoke.");
}

void Pet::_generateVoice() {
    voiceLength = random(2, VOICE_LENGTH_MAX);

    for (uint8_t i = 0; i < voiceLength * 2; i += 2) {
        voice[i] = random(TONE_MIN, TONE_MAX);
        voice[i + 1] = TONE_DURATION_DEFAULT + random(-TONE_DURATION_OFFSET, TONE_DURATION_OFFSET);
    }
}

void Pet::draw() {

    uint8_t drawFace = eyeClosed ? FACE_BLINK : face;

    // Head
    displayDriver->drawBitmap(position.x - SPRITE_WIDTH / 2, position.y - SPRITE_HEIGHT / 2, sprite_heads[head], HEAD_WIDTH, HEAD_HEIGHT, SSD1306_INVERSE);
    // Body
    displayDriver->drawBitmap(position.x - SPRITE_WIDTH / 2, position.y, sprite_bodies[body], BODY_WIDTH, BODY_HEIGHT, SSD1306_INVERSE);
    // Face
    displayDriver->drawBitmap(position.x - SPRITE_WIDTH / 4, position.y - SPRITE_HEIGHT / 4, sprite_faces[drawFace], FACE_WIDTH, FACE_HEIGHT, SSD1306_INVERSE);

    if (isHighlighted) {
        displayDriver->drawRect(position.x - SPRITE_WIDTH / 2, position.y - SPRITE_HEIGHT / 2, 32, 32, SSD1306_INVERSE);
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