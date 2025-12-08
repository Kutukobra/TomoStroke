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
        if (_petsMap[i].petId != "") {
            if (millis() - _petsMap[i].ttl >= PET_TTL) {
                _removePet(_petsMap[i].petId);
            } else {
                _petsMap->pet->update();
                _petsMap->pet->draw();
            }
        }
    }
}

void Orchestrator::updatePet(String petId, PetState state) {
    for (uint8_t i = 0; i < FRIEND_PET_MAX; i++) {
        if (_petsMap[i].petId == petId) {
            _petsMap[i].ttl = millis();
            _loadPetState(_petsMap[i].pet, state);

            return;
        }
    }
    _addPet(petId, state);
}

void Orchestrator::_addPet(String petId, PetState initial) {
    Serial.println("Adding new pet!");

    uint8_t i = 0;
    for (; i < FRIEND_PET_MAX; i++) {
        if (_petsMap[i].petId == "") {
            break;
        }
    }
    
    _petsMap[i].petId = petId;
    _petsMap[i].ttl = millis();
    _loadPetState(_petsMap[i].pet, initial);

    _petCount++;
    return;
}

bool Orchestrator::_removePet(String petId) {
    Serial.println("Removing pet: " + petId);    
    for (uint8_t i = 0; i < FRIEND_PET_MAX; i++) {
        if (_petsMap[i].petId == petId) {
            _petsMap[i].petId = "";
            _petCount--;
            return true;
        }
    }
    return false;
}

void Orchestrator::_loadPetState(Pet* pet, PetState state) {
    pet->setLooks(state.looks.bodyId, state.looks.headId);
    pet->setAttributes(state.attributes.speakInterval, state.attributes.blinkInterval, state.attributes.walkRate, state.attributes.voiceLength, state.attributes.voice);
    pet->setData(state.data.satiation, state.data.happiness);
}

uint8_t Orchestrator::getPetCount() {
    return _petCount;
}

// First pet is pet 1 (assume pet 0 is local pet)
PetMap Orchestrator::getPetMap(uint8_t index) {
    index--;
    if (index >= _petCount) return {};

    uint8_t activeCount = 0;
    for (uint8_t i = 0; i < FRIEND_PET_MAX; i++) {
        if (_petsMap[i].petId != "") {
            if (activeCount == index) {
                return _petsMap[i];
            }
            activeCount++;
        }
    }
    return {};
}