#ifndef PET_HPP
#define PET_HPP

#include <Arduino.h>
#include <Sprites.hpp>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

typedef struct Vector2D {
    int8_t x, y;
} Vector2D;

#define GRAVITY 3

#define BLINK_MIN 1000
#define BLINK_MAX 5000
#define BLINK_CLOSED_DEFAULT 100

#define SPEAK_MIN 12000
#define SPEAK_MAX 120000
#define SPEAK_INTERVAL_OFFSET 3000

#define VOICE_LENGTH_MAX 6
#define TONE_MIN 1000
#define TONE_MAX 2000
#define TONE_DURATION_DEFAULT 100
#define TONE_DURATION_OFFSET 40
#define TONE_HUNGRY_OFFSET 500
#define TONE_LONELY_OFFSET -900

#define MAX_SATIATION 1000
#define HUNGER_RATE 1000
#define HUNGER_DECAY 1
#define HUNGER_WALK 6

#define MAX_HAPPINESS 1000

typedef struct VoiceMessage {
    uint16_t *voice;
    uint8_t voiceLength;
    int8_t toneOffset;
} VoiceMessage;

class Pet {
    Adafruit_SSD1306 *displayDriver;
    QueueHandle_t voiceQueue;

    uint8_t body, head, face = FACE_IDLE; // Look index

    Vector2D position, velocity = {0, 0};

    uint64_t blinkLast = 0, blinkInterval;
    bool eyeClosed = false;
    
    void _blinkCheck();

    uint64_t speakLast = 0, speakInterval;
    void _speakCheck();
    bool isSpeaking = false;
    uint8_t voiceLength;
    uint16_t voice[VOICE_LENGTH_MAX * 2]; // Part pertama frekuensi, part dua duration
    void _generateVoice();

    uint8_t walkRate = 0;
    Vector2D walkSetpoint;
    void _walkCheck();

    uint16_t happiness = MAX_HAPPINESS;
    uint16_t satiation = MAX_SATIATION;
    uint64_t hungerLast = 0;
    void _satiationReduction(uint16_t value);
    void _satiationCheck();
    
    bool isHighlighted = false;
    
    public:
        Pet(Adafruit_SSD1306 *display, QueueHandle_t voiceQueue);
        
        void update();
        void draw();

        void setLooks(uint8_t body, uint8_t head);

        void setIntervals(uint64_t blinkInterval, uint64_t speakInterval);
        
        void setPosition(int8_t x, int8_t y);
        Vector2D getPosition();
        
        void setVoice(uint16_t voice[VOICE_LENGTH_MAX * 2]);
        void speak(int16_t toneOffset = 0);

        uint16_t getHappiness();
        uint16_t getSatiation();
        void feed(uint8_t value);

        void setHighlight(bool isHighlighted);
        void toggleHighlight();

    static Vector2D GetRandomPosition();
};



#endif