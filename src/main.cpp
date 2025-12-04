#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Pet.hpp>
#include <Bar.hpp>
#include <Icon.hpp>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SPRITE_HEIGHT 16
#define SPRITE_WIDTH 16

#define BUTTON_A 16
#define BUZZER 4
#define VIBRATION_SENSOR 5

#define INPUT_DEBOUNCE 100

#define PET_COUNT 8

Pet *pets[PET_COUNT];
Icon hungerIcon(&display, 0, 0);
Bar hungerBar(&display, 17, 1, 46, 12, MAX_SATIATION, MAX_SATIATION);
Icon happinessIcon(&display, 65, 0);
Bar happinessBar(&display, 82, 1, 46, 12, MAX_HAPPINESS, MAX_HAPPINESS);

uint16_t feedingSound[] = {100, 20};

QueueHandle_t voiceQueue;

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
    uint8_t currentPet = PET_COUNT;

    while (1) {
        hungerBar.setCapacity(pets[currentPet >= PET_COUNT ? 0 : currentPet]->getSatiation()); // Satu siklus ga ada yang dihighlight samsek
        happinessBar.setCapacity(pets[currentPet >= PET_COUNT ? 0 : currentPet]->getHappiness()); 

        if (currentPet != PET_COUNT) {
            uint8_t face = pets[currentPet]->getFace();

            uint8_t faceIcon;
            if (face == FACE_HAPPY) {
                faceIcon = ICON_HAPPY;
            } else if (face == FACE_SAD) {
                faceIcon = ICON_SAD;
            } else if (face == FACE_BLINK) {
                faceIcon = ICON_BORED;
            }
            happinessIcon.setIcon(faceIcon);

            hungerIcon.setIcon(pets[currentPet]->isHungry() ? ICON_HUNGRY : ICON_SATIATED);
        }

        if (digitalRead(BUTTON_A) == LOW && millis() - lastDebounce > INPUT_DEBOUNCE) {
            lastDebounce = millis();
            
            if (currentPet < PET_COUNT)
                pets[currentPet]->setHighlight(false);
            currentPet++;

            if (currentPet > PET_COUNT) currentPet = 0;

            if (currentPet < PET_COUNT)
                pets[currentPet]->setHighlight(true);
        }
    
        if (currentPet != PET_COUNT && digitalRead(VIBRATION_SENSOR) == HIGH && millis() - lastVibration > INPUT_DEBOUNCE) {
            lastVibration = millis();
            pets[currentPet]->feed(50);
        }
        
        display.clearDisplay();
        for (int i = 0; i < PET_COUNT; i++) {
            pets[i]->update();
            pets[i]->draw();
        }

        if (currentPet != PET_COUNT)
            GUI::DrawAll();

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

    for (int i = 0; i < PET_COUNT; i++) {
        pets[i] = new Pet(&display, voiceQueue);
    }

    xTaskCreate(MainLoop, "Main Loop", 8192, NULL, 2, NULL);
    xTaskCreate(VoiceTask, "Voice Processing Task", 2048, NULL, 1, NULL);
}

void loop()
{
    // Jangan taro apapun di sini
}
