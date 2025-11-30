#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SPRITE_HEIGHT 16
#define SPRITE_WIDTH 16
static const uint8_t PROGMEM bmp[] {
	0x00, 0x00, 0x0f, 0xf0, 0x10, 0x08, 0x20, 0x04, 0x24, 0x24, 0x20, 0x04, 0x23, 0xc4, 0x20, 0x04, 
	0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x00
};

#define BUTTON_A 16
#define BUZZER 4

void setup()
{
    Serial.begin(115200);

    pinMode(BUTTON_A, INPUT_PULLUP);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }
}


uint8_t x = 56, y = 16;
int8_t vel_x = 0, vel_y = 0;
void loop()
{
    display.clearDisplay();
    display.drawBitmap(x, y, bmp, SPRITE_WIDTH, SPRITE_HEIGHT, SSD1306_WHITE);
    delay(50);
    vel_y += 3;
    if (y + SPRITE_HEIGHT >= SCREEN_HEIGHT) {
        vel_y = 0;
        y = SCREEN_HEIGHT - SPRITE_HEIGHT;
    }

    if (digitalRead(BUTTON_A) == LOW && vel_y == 0) {
        vel_y = -10;
    }
    
    y += vel_y;
    display.display();
}
