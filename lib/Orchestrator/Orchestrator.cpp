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
        // Pet *p = _petsMap[i].pet;
        // Serial.printf("Pet %s\n\t-Looks: %d %d\n\t-Data: %d %d\n", 
        //     _petsMap[i].petId, 
        //     p->getLooks().headId, p->getLooks().bodyId,
        //     p->getData().satiation, p->getData().happiness
        // );
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
    _loadPetState(_petsMap[_petCount].pet, initial);

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

void Orchestrator::_loadPetState(Pet* pet, PetState state) {
    pet->setLooks(state.looks.headId, state.looks.bodyId);
    pet->setAttributes(state.attributes.speakInterval, state.attributes.blinkInterval, state.attributes.walkRate, state.attributes.voiceLength, state.attributes.voice);
    pet->setData(state.data.satiation, state.data.happiness);
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