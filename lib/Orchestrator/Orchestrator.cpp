#include <Orchestrator.hpp>

Orchestrator::Orchestrator(Adafruit_SSD1306 *display, QueueHandle_t *voiceQueue)
    : _display(display), _voiceQueue(voiceQueue) {
    _petCount = 0;
}

void Orchestrator::update() {
    for (uint8_t i = 0; i < _petCount; i++) {
        if (_petsMap[i].petId != "") {
            if (millis() - _petsMap[i].ttl < PET_TTL) {
                _petsMap->pet->update();
                _petsMap->pet->draw();
            } else {
                removePet(_petsMap[i].petId);
            }
        }
    }
}

Pet* Orchestrator::updatePet(String petId, uint16_t ttl, PetState state) {
    for (uint8_t i = 0; i < _petCount; i++) {
        if (_petsMap[i].petId == petId) {
            _petsMap[i].ttl = ttl;
            _loadPetState(_petsMap[i].pet, state);

            return _petsMap[i].pet;
        }
    }
    return addPet(petId, ttl, state);
}

Pet* Orchestrator::addPet(String petId, uint16_t ttl, PetState initial) {
    Serial.println("Adding new pet!");

    for (uint8_t i = 0; i < _petCount; i++) {
        if (_petsMap[i].petId == "") {
            _petsMap[i].petId = petId;
            _petsMap[i].ttl = ttl;
            _petsMap[i].pet = new Pet(_display, *_voiceQueue);
            if (_petsMap[i].pet == nullptr) {
                Serial.println("Failed to create pet instance");
                return nullptr;
            }
            _loadPetState(_petsMap[i].pet, initial);

            return _petsMap[i].pet;
        }
    }

    _petsMap[_petCount].petId = petId;
    _petsMap[_petCount].ttl = ttl;
    _petsMap[_petCount].pet = new Pet(_display, *_voiceQueue);
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

void Orchestrator::_loadPetState(Pet* pet, PetState state) {
    pet->setLooks(state.looks.bodyId, state.looks.headId);
    pet->setAttributes(state.attributes.speakInterval, state.attributes.blinkInterval, state.attributes.walkRate, state.attributes.voiceLength, state.attributes.voice);
    pet->setData(state.data.satiation, state.data.happiness);
}