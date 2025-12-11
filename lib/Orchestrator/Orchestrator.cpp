#include <Orchestrator.hpp>

Orchestrator::Orchestrator(Pet *localPet, Adafruit_SSD1306 *display, QueueHandle_t *voiceQueue)
    : _display(display), _voiceQueue(voiceQueue) {
    _petCount = 1;
    _petsMap[0].pet = localPet;

    for (uint8_t i = 1; i < PET_COUNT; i++) {
        _petsMap[i].pet = new Pet(display, voiceQueue);
    }
}

void Orchestrator::update() {
    for (uint8_t i = 1; i < _petCount; i++) {
        if (millis() - _petsMap[i].ttl >= PET_TTL) {
            if (_removePet(i) == true)
                i--;
        } else {
            _petsMap[i].pet->update();
            _petsMap[i].pet->draw();
        }
    }
    _petsMap[0].pet->update();
    _petsMap[0].pet->draw();
}

void Orchestrator::updatePet(String petId, PetState state) {
    for (uint8_t i = 1; i < _petCount; i++) {
        if (_petsMap[i].petId == petId) {
            _petsMap[i].ttl = millis();
            _petsMap[i].pet->setData(state.data.satiation, state.data.happiness);
            return;
        }
    }
    _addPet(petId, state);
}

void Orchestrator::_addPet(String petId, PetState initial) {
    if (_petCount >= PET_COUNT - 1) return;
    
    _petsMap[_petCount].petId = petId;
    _petsMap[_petCount].ttl = millis();
    
    Serial.printf("Adding new pet: %d %d (%d)\n", initial.looks.headId, initial.looks.bodyId, _petsMap[_petCount].ttl);

    Pet *pet = _petsMap[_petCount].pet;
    pet->setLooks(initial.looks.headId, initial.looks.bodyId);
    pet->setAttributes(initial.attributes.speakInterval, initial.attributes.blinkInterval, initial.attributes.walkRate, initial.attributes.voiceLength, initial.attributes.voice);
    pet->setData(initial.data.satiation, initial.data.happiness);

    _petCount++;
    return;
}

bool Orchestrator::_removePet(uint8_t index) {
    Serial.printf("Removing pet %d (%d)\n",index, millis());    

    if (index >= PET_COUNT) return false;

    for (uint8_t i = index; i < PET_COUNT - 1; i++) {
        _petsMap[i] = _petsMap[i + 1];
    }
    _petsMap[PET_COUNT - 1].petId = "";

    _petCount--;
    return true;
}


uint8_t Orchestrator::getPetCount() {
    return _petCount;
}

PetMap Orchestrator::getPetMap(uint8_t index) {
    if (index >= _petCount) return _petsMap[0];

    return _petsMap[index];
}