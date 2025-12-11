#ifndef ORCHESTRATOR_HPP
#define ORCHESTRATOR_HPP

#include <Orchestrator.hpp>
#include <Pet.hpp>
#include <MeshController.hpp>

#include <Arduino.h>
#include <Wire.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define PET_COUNT 10

typedef struct PetMap {
    String petId;
    Pet* pet;
    uint16_t ttl;
} PetMap;

class Orchestrator {
public:
    Orchestrator(Pet *localPet, Adafruit_SSD1306 *display, QueueHandle_t *voiceQueue);
    void update();
    void updatePet(String petId, PetState state);

    PetMap getPetMap(uint8_t index);
    uint8_t getPetCount();
    
private:
    PetMap _petsMap[PET_COUNT];
    uint8_t _petCount;
    
    Adafruit_SSD1306 *_display;
    QueueHandle_t *_voiceQueue;
    
    void _addPet(String petId, PetState initial);
    bool _removePet(uint8_t index);

};

#endif