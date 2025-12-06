#include <Orchestrator.hpp>

Orchestrator::Orchestrator(Adafruit_SSD1306 &display, QueueHandle_t voiceQueue)
    : _display(display), _voiceQueue(voiceQueue) {
    _petCount = 0;
}

void Orchestrator::update(uint64_t deltaTime) {
    for (uint8_t i = 0; i < _petCount; i++) {
        if (_petsMap[i].petId != "") {
            if (_petsMap[i].ttl > deltaTime) {
                _petsMap[i].ttl -= deltaTime;
            } else {
                removePet(_petsMap[i].petId);
            }
        }
    }
}

Pet* Orchestrator::addPet(String petId, uint16_t ttl, PetState initial, Adafruit_SSD1306 &display, QueueHandle_t voiceQueue) {
    for (uint8_t i = 0; i < _petCount; i++) {
        if (_petsMap[i].petId == "") {
            _petsMap[i].petId = petId;
            _petsMap[i].state = initial;
            _petsMap[i].ttl = ttl;
            _petsMap[i].pet = new Pet(&display, voiceQueue);
            if (_petsMap[i].pet == nullptr) {
                Serial.println("Failed to create pet instance");
            }
            _loadPetState(_petsMap[i].pet, initial);

            return _petsMap[i].pet;
        }
    }

    _petsMap[_petCount].petId = petId;
    _petsMap[_petCount].state = initial;
    _petsMap[_petCount].ttl = ttl;
    _petsMap[_petCount].pet = new Pet(&display, voiceQueue);
    _petCount++;

    return _petsMap[_petCount - 1].pet;
}

bool Orchestrator::removePet(String petId) {
    for (uint8_t i = 0; i < _petCount; i++) {
        if (_petsMap[i].petId == petId) {
            _petsMap[i].petId = "";
            delete _petsMap[i].pet;
            _petCount--;
            return true;
        }
    }
    return false;
}

Pet* Orchestrator::updatePet(String petId, uint16_t ttl, PetState state, Adafruit_SSD1306 &display, QueueHandle_t voiceQueue) {
    for (uint8_t i = 0; i < _petCount; i++) {
        if (_petsMap[i].petId == petId) {
            _petsMap[i].state = state;
            _petsMap[i].ttl = ttl;
            _loadPetState(_petsMap[i].pet, state);

            return _petsMap[i].pet;
        }
    }
    return addPet(petId, ttl, state, display, voiceQueue);
}

void Orchestrator::clear() {
    _petCount = 0;
}




void Orchestrator::_loadPetState(Pet* pet, PetState state) {
    pet->setLooks(state.body, state.head);
    pet->setIntervals(state.blinkInterval, state.speakInterval);
    pet->setVoice(state.voice);
    pet->setWalkRate(state.walkRate);
    pet->setHappiness(state.happiness);
    pet->setSatiation(state.satiation);
}