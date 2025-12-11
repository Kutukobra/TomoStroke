#include <Pet.hpp>

Pet::Pet(Adafruit_SSD1306 *display, QueueHandle_t *voiceMessageQueue) : displayDriver(display), voiceQueue(voiceMessageQueue) {
    bodyId = random(0, BODY_COUNT);
    headId = random(0, HEAD_COUNT);
    blinkInterval = random(BLINK_MIN, BLINK_MAX);

    speakInterval = random(SPEAK_MIN, SPEAK_MAX);

    position = GetRandomPosition();

    walkRate = random(1, 101);

    _generateVoice();
}

PetLooks Pet::getLooks() {
    return {
        headId: this->headId, bodyId: this->bodyId
    };
} 

void Pet::setLooks(uint8_t head, uint8_t body) {
    this->headId = head;
    this->bodyId = body;
}

void Pet::setAttributes(uint64_t speakInterval, uint64_t blinkInterval, uint8_t walkRate, uint8_t voiceLength, uint16_t voice[]) {
    this->speakInterval = speakInterval;
    this->blinkInterval = blinkInterval;
    this->walkRate = walkRate;
    this->voiceLength = voiceLength;

    for (int i = 0; i < voiceLength * 2; i+=2) {
        this->voice[i] = voice[i];
        this->voice[i + 1] = voice[i + 1];
    }
}

PetAttributes Pet::getAttributes() {
    PetAttributes attributes;
    attributes.speakInterval = this->speakInterval;
    attributes.blinkInterval = this->blinkInterval;
    attributes.walkRate =  this->walkRate;
    attributes.voiceLength = this->voiceLength;

    for (int i = 0; i < voiceLength * 2; i+=2) {
        attributes.voice[i] = voice[i];
        attributes.voice[i + 1] = voice[i + 1];
    }

    return attributes;
}

void Pet::setData(int16_t satiation, int16_t happiness) {
    this->satiation = satiation;
    this->happiness = happiness;
}

PetData Pet::getData() {
    return {
        satiation: this->satiation, happiness: this->happiness
    };
}

void Pet::update() {
    _happinessCheck();
    _satiationCheck();
    _blinkCheck();
    _speakCheck();
    _walkCheck();

    _faceCheck();

    velocity = {0, 0};

    if (walkSetpoint.x > position.x) {
        velocity.x = 1;
    } else if (walkSetpoint.x < position.x) {
        velocity.x = -1;
    }

    if (walkSetpoint.y > position.y) {
        velocity.y = 1;
    } else if (walkSetpoint.y < position.y) {
        velocity.y = -1;
    }

    if (!isSpeaking) {
        position.x += velocity.x;
        position.y += velocity.y;
    }

    if (position.y - SPRITE_HEIGHT / 2 <= 18) { // Limit top bar
        position.y = SPRITE_HEIGHT / 2 + 18;
    } else if (position.y + SPRITE_HEIGHT / 2 >= 64) {
        position.y = 63 - SPRITE_HEIGHT / 2;
    }

    if (position.x + SPRITE_WIDTH / 2 >= 128) {
        position.x = 128 - SPRITE_WIDTH / 2;
    } else if (position.x - SPRITE_WIDTH / 2 <= 0) {
        position.x = SPRITE_WIDTH / 2;
    }
}

void Pet::_faceCheck() {
    face = FACE_IDLE;
    
    if (isBlinking) {
        face = FACE_BLINK;
        return;
    }

    if (happiness <= MAX_HAPPINESS * 4 / 10) {
        face = FACE_SAD;
    } else if (happiness <= MAX_HAPPINESS * 6 / 10) {
        face = FACE_BLINK;
    } else if (satiation >= MAX_SATIATION * 95 / 100) {
        face = FACE_HAPPY;
    }
    
    if (hungry && random(0, 6)) {
        face = FACE_SURPRISED;
    }
}

void Pet::_blinkCheck() {
    uint64_t currentTime = millis();

    if (isBlinking) {
        if (currentTime - blinkLast >= BLINK_CLOSED_DEFAULT + blinkInterval / 10) {
            isBlinking = false;
            blinkLast = currentTime;
        }
    } else {
        if (currentTime - blinkLast >= blinkInterval) {
            isBlinking = true;
            blinkLast = currentTime;
        }
    }
}

void Pet::_speakCheck() {
    uint64_t currentTime = millis();

    uint64_t speakIntervalModified = (hungry ? speakInterval / 10 : speakInterval);

    if (isSpeaking && (currentTime - speakLast >= 200 * voiceLength)) {
        isSpeaking = false;
    }

    if (currentTime - speakLast > speakIntervalModified) {
        speak(hungry ? TONE_HUNGRY_OFFSET : 0);
        speakLast = currentTime + random(-SPEAK_INTERVAL_OFFSET, SPEAK_INTERVAL_OFFSET);
    }
}

void Pet::_walkCheck() {
    uint16_t randomChance = random(1, 6001);
    if (walkRate >= randomChance) {
        _satiationReduction(HUNGER_WALK);
        walkSetpoint = GetRandomPosition();
    }
}

void Pet::_satiationCheck() {
    uint64_t currentTime = millis();
    if (currentTime - hungerLast >= HUNGER_RATE) {
        _satiationReduction(HUNGER_DECAY);
        hungerLast = currentTime;
    }
    hungry = satiation <= MAX_SATIATION / 3;
}

void Pet::_happinessCheck() {
    uint64_t currentTime = millis();
    if (currentTime - lonelyLast >= HAPPINESS_RATE) {
        _happinessReduction(HAPPINESS_DECAY);
        lonelyLast = currentTime;
    }   
}

void Pet::speak(int16_t toneOffset) {
    isSpeaking = true;
    speakLast = millis();
    VoiceMessage message;
    message.voiceLength = voiceLength;
    message.voice = voice;
    message.toneOffset = toneOffset;
    xQueueSend(*voiceQueue, &message, 10);
    Serial.println("Pet spoke!");
}

void Pet::_generateVoice() {
    voiceLength = random(2, VOICE_LENGTH_MAX);

    for (uint8_t i = 0; i < voiceLength * 2; i += 2) {
        voice[i] = random(TONE_MIN, TONE_MAX);
        voice[i + 1] = TONE_DURATION_DEFAULT + random(-TONE_DURATION_OFFSET, TONE_DURATION_OFFSET);
    }
}

void Pet::draw() {
    // Head
    displayDriver->drawBitmap(position.x - SPRITE_WIDTH / 2, position.y - SPRITE_HEIGHT / 2, sprite_heads[headId], HEAD_WIDTH, HEAD_HEIGHT, SSD1306_INVERSE);
    // Body
    displayDriver->drawBitmap(position.x - SPRITE_WIDTH / 2, position.y, sprite_bodies[bodyId], BODY_WIDTH, BODY_HEIGHT, SSD1306_INVERSE);
    // Face
    displayDriver->drawBitmap(position.x - SPRITE_WIDTH / 4, position.y - SPRITE_HEIGHT / 4, sprite_faces[face], FACE_WIDTH, FACE_HEIGHT, SSD1306_INVERSE);
}

void Pet::drawHighlight() {
    displayDriver->drawRect(position.x - SPRITE_WIDTH / 2 - 2, position.y - SPRITE_HEIGHT / 2 - 2, 20, 20, SSD1306_INVERSE);
}

uint8_t Pet::getFace() {
    return face;
}

bool Pet::isHungry() {
    return hungry;
}

void Pet::_satiationReduction(uint16_t value) {
    satiation -= value;
    if (satiation <= 1) satiation = 1;
}

void Pet::_happinessReduction(uint16_t value) {
    happiness -= value;
    if (happiness <= 1) happiness = 1;
}

void Pet::feed(uint8_t value) {
    if (satiation < MAX_SATIATION) {
        VoiceMessage feeding;
        feeding.voiceLength = 1;
        feeding.voice = voice;
        xQueueSend(*voiceQueue, &feeding, 10);
    }
    
    satiation += value;
    if (satiation >= MAX_SATIATION) satiation = MAX_SATIATION;
}

void Pet::addHappiness(uint8_t value) {
    happiness += value;
    if (happiness > MAX_HAPPINESS) {
        happiness = MAX_HAPPINESS;
    } 
}

Vector2D Pet::GetRandomPosition() {
    int8_t x = random(SPRITE_WIDTH / 2, 129 - SPRITE_WIDTH / 2);
    int8_t y = random(SPRITE_HEIGHT / 2, 65 - SPRITE_HEIGHT / 2);
    return (Vector2D){x, y};
}