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

Pet *pet;

void setup()
{
    Serial.begin(115200);

    pinMode(BUTTON_A, INPUT_PULLUP);

    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }

    pet = new Pet(&display);

    randomSeed(analogRead(0));
}


void loop()
{
    if (digitalRead(BUTTON_A) == LOW) {
        Serial.println("Pet Jumped!");
        pet->jump();
    }
    
    display.clearDisplay();
    pet->update();
    pet->draw();
    display.display();

    delay(40);
}
