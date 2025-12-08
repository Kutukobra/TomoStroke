#include <Orchestrator.hpp>

Orchestrator::Orchestrator(Adafruit_SSD1306 *display, QueueHandle_t *voiceQueue)
    : _display(display), _voiceQueue(voiceQueue) {
    _petCount = 0;
    for (uint8_t i = 0; i < FRIEND_PET_MAX; i++) {
        _petsMap[i].pet = new Pet(display, voiceQueue);
    }
}

void Orchestrator::update() {
    for (uint8_t i = 0; i < _petCount; i++) {
        if (millis() - _petsMap[i].ttl >= PET_TTL) {
            _removePet(_petsMap[i].petId);
        } else {
            _petsMap[i].pet->update();
            _petsMap[i].pet->draw();
        }
    }
}

void Orchestrator::updatePet(String petId, PetState state) {
    for (uint8_t i = 0; i < _petCount; i++) {
        if (_petsMap[i].petId == petId) {
            _petsMap[i].ttl = millis();
            _petsMap[i].pet->setData(state.data.satiation, state.data.happiness);
            return;
        }
    }
    _addPet(petId, state);
}

void Orchestrator::_addPet(String petId, PetState initial) {
    Serial.printf("Adding new pet: %d %d\n", initial.looks.headId, initial.looks.bodyId);

    _petsMap[_petCount].petId = petId;
    _petsMap[_petCount].ttl = millis();
    Pet *pet = _petsMap[_petCount].pet;

    pet->setLooks(initial.looks.headId, initial.looks.bodyId);
    pet->setAttributes(initial.attributes.speakInterval, initial.attributes.blinkInterval, initial.attributes.walkRate, initial.attributes.voiceLength, initial.attributes.voice);

    _petCount++;
    return;
}

bool Orchestrator::_removePet(String petId) {
    Serial.println("Removing pet: " + petId);    
    uint8_t i;
    for (i = 0; i < FRIEND_PET_MAX; i++) {
        if (_petsMap[i].petId == petId) {
            break;
        }
    }

    for (; i < FRIEND_PET_MAX - 1; i++) {
        _petsMap[i] = _petsMap[i + 1];
    }
    _petsMap[i + 1].petId = "";

    return false;
}
uint8_t Orchestrator::getPetCount() {
    return _petCount;
}

// First pet is pet 1 (assume pet 0 is local pet)
PetMap Orchestrator::getPetMap(uint8_t index) {
    index--;
    if (index <= 0) {
        return _petsMap[0];
    } else if (index >= _petCount) {
        return _petsMap[_petCount - 1];
    }
    return _petsMap[index - 1];
}