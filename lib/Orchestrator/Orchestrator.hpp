#ifndef ORCHESTRATOR_HPP
#define ORCHESTRATOR_HPP

#include <Orchestrator.hpp>
#include <Pet.hpp>

#include <Arduino.h>
#include <Wire.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define VOICE_LENGTH_MAX 6

typedef struct PetsMap {
    String petId;
    Pet* pet;
    uint16_t ttl;
    PetState state;
} PetsMap;

class Orchestrator {
public:
    Orchestrator(Adafruit_SSD1306 &display, QueueHandle_t voiceQueue);
    void update(uint64_t deltaTime);

    Pet* addPet(String petId, uint16_t ttl, PetState initial, Adafruit_SSD1306 &display, QueueHandle_t voiceQueue);
    bool removePet(String petId);
    Pet* updatePet(String petId, uint16_t ttl, PetState state, Adafruit_SSD1306 &display, QueueHandle_t voiceQueue);

    void clear();

private:
    PetsMap _petsMap[10];
    uint8_t _petCount;

    Adafruit_SSD1306 &_display;
    QueueHandle_t _voiceQueue;


    void _loadPetState(Pet* pet, PetState state);
};

#endif