#include <Pet.hpp>

Pet::Pet(Adafruit_SSD1306 *display, QueueHandle_t voiceMessageQueue) : displayDriver(display), voiceQueue(voiceMessageQueue) {
    body = random(0, BODY_COUNT);
    head = random(0, HEAD_COUNT);
    blinkInterval = random(BLINK_MIN, BLINK_MAX);

    speakInterval = random(SPEAK_MIN, SPEAK_MAX);

    position = GetRandomPosition();

    walkRate = random(1, 101);

    _generateVoice();

    Serial.printf("- New Pet\n\tPosition: %d %d\n\tBody: %d\n\tFace: %d\n\tBlink Interval: %d\n\tWalk Rate: %d\n", position.x, position.y, body, face, blinkInterval, walkRate);
    Serial.printf("\tVoice (Interval=%d):\n", speakInterval);
    for (uint8_t i = 0; i < voiceLength * 2; i += 2) {
        Serial.printf("\t\tF: %d, D: %d\n", voice[i], voice[i + 1]);
    }
}

void Pet::setLooks(uint8_t body, uint8_t head) {
    this->body = body;
    this->head = head;
}


void Pet::setIntervals(uint64_t blinkInterval, uint64_t speakInterval) {
    this->blinkInterval = blinkInterval;
    this->speakInterval = speakInterval;
}

void Pet::setVoice(uint16_t voice[VOICE_LENGTH_MAX * 2]) {
    for (int i = 0; i < VOICE_LENGTH_MAX * 2; i++) {
        this->voice[i] = voice[i];
    }
}

void Pet::update() {
    _blinkCheck();
    _speakCheck();
    _walkCheck();

    if (walkSetpoint.x > position.x) {
        position.x += 1;
    } else if (walkSetpoint.x < position.x) {
        position.x -= 1;
    }

    if (walkSetpoint.y > position.y) {
        position.y += 1;
    } else if (walkSetpoint.y < position.y) {
        position.y -= 1;
    }

    if (position.y - SPRITE_HEIGHT / 2 <= 16) { // Limit top bar
        position.y = SPRITE_HEIGHT / 2 + 16;
    } else if (position.y + SPRITE_HEIGHT / 2 >= 64) {
        position.y = 63 - SPRITE_HEIGHT / 2;
    }

    if (position.x + SPRITE_WIDTH / 2 >= 128) {
        position.x = 128 - SPRITE_WIDTH / 2;
    } else if (position.x - SPRITE_WIDTH / 2 <= 0) {
        position.x = SPRITE_WIDTH / 2;
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

    if (isSpeaking && (currentTime - speakLast >= 100 * voiceLength)) {
        isSpeaking = false;
    }

    if (currentTime - speakLast >= speakInterval) {
        speak();
        speakLast = currentTime + random(-SPEAK_INTERVAL_OFFSET, SPEAK_INTERVAL_OFFSET);
    }
}

void Pet::_walkCheck() {
    uint16_t randomChance = random(1, 3001);
    if (walkRate >= randomChance) {
        walkSetpoint = GetRandomPosition();
    }
}

void Pet::speak() {
    isSpeaking = true;
    speakLast = millis();
    VoiceMessage message;
    message.voiceLength = voiceLength;
    message.voice = voice;
    xQueueSend(voiceQueue, &message, 10);
}

void Pet::_generateVoice() {
    voiceLength = random(2, VOICE_LENGTH_MAX);

    for (uint8_t i = 0; i < voiceLength * 2; i += 2) {
        voice[i] = random(TONE_MIN, TONE_MAX);
        voice[i + 1] = TONE_DURATION_DEFAULT + random(-TONE_DURATION_OFFSET, TONE_DURATION_OFFSET);
    }
}

void Pet::draw() {

    uint8_t drawFace = face;

    if (eyeClosed) drawFace = FACE_BLINK;
    if (isSpeaking) drawFace = FACE_SURPRISED;

    // Head
    displayDriver->drawBitmap(position.x - SPRITE_WIDTH / 2, position.y - SPRITE_HEIGHT / 2, sprite_heads[head], HEAD_WIDTH, HEAD_HEIGHT, SSD1306_INVERSE);
    // Body
    displayDriver->drawBitmap(position.x - SPRITE_WIDTH / 2, position.y, sprite_bodies[body], BODY_WIDTH, BODY_HEIGHT, SSD1306_INVERSE);
    // Face
    displayDriver->drawBitmap(position.x - SPRITE_WIDTH / 4, position.y - SPRITE_HEIGHT / 4, sprite_faces[drawFace], FACE_WIDTH, FACE_HEIGHT, SSD1306_INVERSE);

    if (isHighlighted) {
        displayDriver->drawRect(position.x - SPRITE_WIDTH / 2 - 2, position.y - SPRITE_HEIGHT / 2 - 2, 20, 20, SSD1306_INVERSE);
    }
}

void Pet::setPosition(int8_t newX, int8_t newY) {
    position = {newX, newY};
}

Vector2D Pet::getPosition() {
    return position;
}

void Pet::setHighlight(bool highlighted) {
    isHighlighted = highlighted;
}

void Pet::toggleHighlight() {
    isHighlighted = !isHighlighted;
}

Vector2D Pet::GetRandomPosition() {
    int8_t x = random(SPRITE_WIDTH / 2, 129 - SPRITE_WIDTH / 2);
    int8_t y = random(SPRITE_HEIGHT / 2, 65 - SPRITE_HEIGHT / 2);
    return (Vector2D){x, y};
}