#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Pet.hpp>
#include <Bar.hpp>
#include <Icon.hpp>

#include <MeshController.hpp>
#include <Orchestrator.hpp>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SPRITE_HEIGHT 16
#define SPRITE_WIDTH 16

#define BUTTON_A 5
#define BUZZER 4
#define VIBRATION_SENSOR 17

#define INPUT_DEBOUNCE 100


Pet *localPet;

Icon hungerIcon(&display, 0, 0);
Bar hungerBar(&display, 17, 1, 46, 12, MAX_SATIATION, MAX_SATIATION);
Icon happinessIcon(&display, 65, 0);
Bar happinessBar(&display, 82, 1, 46, 12, MAX_HAPPINESS, MAX_HAPPINESS);

QueueHandle_t voiceQueue;

QueueHandle_t inPetPacketQueue;
QueueHandle_t inFriendFeedQueue;

MeshController meshController(&inPetPacketQueue, &inFriendFeedQueue);

Orchestrator orchestrator(&display, &voiceQueue);

void FriendFeedTask(void*) {
    uint8_t feedingValue;
    while (1) {
        if (xQueueReceive(inFriendFeedQueue, &feedingValue, portMAX_DELAY) == pdTRUE) {
            Serial.println("Fed: " + feedingValue);
            localPet->feed(feedingValue);
        } 
    }
}

void PetPacketTask(void*) {
    PetPacket petPacket;
    while (1) {
        if (xQueueReceive(inPetPacketQueue, &petPacket, portMAX_DELAY) == pdTRUE) {
            orchestrator.updatePet(petPacket.mac, petPacket.state);
        }
    }
}

void BroadcastTask(void *) {
    while (1) {
        PetState petState;
        petState.looks = localPet->getLooks();
        petState.attributes = localPet->getAttributes();
        petState.data = localPet->getData();
        meshController.broadcast(petState);

        vTaskDelay(PET_TTL / 2);
    }
}

void VoiceTask(void*) {
    VoiceMessage message;

    while (1) {
        if (xQueueReceive(voiceQueue, &message, portMAX_DELAY) == pdTRUE) {
            for (uint8_t i = 0; i < message.voiceLength * 2; i += 2) {
                tone(BUZZER, message.voice[i] + message.toneOffset, message.voice[i + 1]);
                vTaskDelay(message.voice[i + 1]);
            }
        }
        noTone(BUZZER);
    }
}

void MainLoop(void *) {
    uint64_t lastDebounce;
    uint64_t lastVibration;

    Pet *selectedPet = localPet;
    PetMap selectedPetMap;
    uint8_t petCount; 
    uint8_t currentPet = 0;

    while (1) {
        display.clearDisplay();
        petCount = orchestrator.getPetCount() + 1;
        if (currentPet >= petCount) {
            currentPet = 0;
        }
        
        if (digitalRead(BUTTON_A) == LOW && millis() - lastDebounce > INPUT_DEBOUNCE) {
            lastDebounce = millis();
            
            currentPet++;
            if (currentPet >= petCount) {
                currentPet = 0;
            }
            // Serial.printf("Selecting: %d\n", currentPet);
        }

        if (currentPet == 0) {
            selectedPet = localPet;
        } else {
            selectedPetMap = orchestrator.getPetMap(currentPet);
            selectedPet = selectedPetMap.pet;
        }

        if (digitalRead(VIBRATION_SENSOR) == HIGH && millis() - lastVibration > INPUT_DEBOUNCE) {
            lastVibration = millis();
            selectedPet->feed(FEEDING_VALUE);
            if (selectedPet != localPet) {
                meshController.feedFriend(selectedPetMap.petId);
            }
        }
        
        { // GUI Update
            PetData currentData;
            currentData = selectedPet->getData(); // Satu siklus ga ada yang dihighlight samsek
            hungerBar.setCapacity(currentData.satiation); 
            happinessBar.setCapacity(currentData.happiness); 

            uint8_t face = selectedPet->getFace();
            happinessIcon.setIcon(Icon::FaceToIcon(face));

            hungerIcon.setIcon(selectedPet->isHungry() ? ICON_HUNGRY : ICON_SATIATED);

            GUI::DrawAll();
        }

        localPet->update();
        orchestrator.update();
        
        localPet->draw();

        selectedPet->drawHighlight();
        display.display();
        vTaskDelay(pdMS_TO_TICKS(40));
    }
}

void setup()
{
    Serial.begin(115200);
    randomSeed(analogRead(36));

    pinMode(BUTTON_A, INPUT_PULLUP);
    pinMode(BUZZER, OUTPUT);
    pinMode(VIBRATION_SENSOR, INPUT);

    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }    
    
    voiceQueue = xQueueCreate(3, sizeof(VoiceMessage));
    inPetPacketQueue = xQueueCreate(5, sizeof(PetPacket));
    inFriendFeedQueue = xQueueCreate(5, sizeof(uint8_t));

    localPet = new Pet(&display, &voiceQueue);

    meshController.setup();

    xTaskCreate(MainLoop, "Main Loop", 8192, NULL, 2, NULL);
    xTaskCreate(VoiceTask, "Voice Processing Task", 2048, NULL, 1, NULL);
    xTaskCreate(BroadcastTask, "Broadcast Task", 8192, NULL, 2, NULL);
    xTaskCreate(PetPacketTask, "Packet Handling Task", 8192, NULL, 1, NULL);
    xTaskCreate(FriendFeedTask, "Fed by Friend Task", 8192, NULL, 1, NULL);
}

void loop()
{
    meshController.update();
}
