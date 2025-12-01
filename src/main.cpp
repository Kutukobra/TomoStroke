#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Pet.hpp>

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

#define PET_COUNT 5

Pet *pets[PET_COUNT];

QueueHandle_t voiceQueue;

void VoiceTask(void*) {
    VoiceMessage message;

    while (1) {
        if (xQueueReceive(voiceQueue, &message, portMAX_DELAY) == pdTRUE) {
            for (uint8_t i = 0; i < message.voiceLength * 2; i += 2) {
                tone(BUZZER, message.voice[i], message.voice[i + 1]);
                vTaskDelay(message.voice[i + 1]);
            }
        }
        noTone(BUZZER);
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
    
    voiceQueue = xQueueCreate(20, sizeof(VoiceMessage));

    for (int i = 0; i < PET_COUNT; i++) {
        pets[i] = new Pet(&display, voiceQueue);
    }

    xTaskCreate(VoiceTask, "Voice Processing Task", 2048, NULL, 1, NULL);
}

void loop()
{
    static uint64_t lastDebounce;
    static uint64_t lastVibration;
    static uint8_t currentPet = 0;

    if (digitalRead(BUTTON_A) == LOW && millis() - lastDebounce > 200) {
        lastDebounce = millis();
        pets[currentPet]->setHighlight(false);
        currentPet++;
        if (currentPet >= PET_COUNT) currentPet = 0;
        pets[currentPet]->setHighlight(true);
    }

    if (digitalRead(VIBRATION_SENSOR) == HIGH && millis() - lastVibration > 200) {
        lastVibration = millis();
        pets[currentPet]->speak();
    }
    
    display.clearDisplay();
    for (int i = 0; i < PET_COUNT; i++) {
        pets[i]->update();
        pets[i]->draw();
    }
    display.display();
    
    delay(80);
}
