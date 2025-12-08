#ifndef PET_HPP
#define PET_HPP

#include <Arduino.h>
#include <Sprites.hpp>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

struct Vector2D {
    int8_t x, y;
};

#define GRAVITY 3

#define BLINK_MIN 1000
#define BLINK_MAX 5000
#define BLINK_CLOSED_DEFAULT 100

#define SPEAK_MIN 30000
#define SPEAK_MAX 360000
#define SPEAK_INTERVAL_OFFSET 3000

#define VOICE_LENGTH_MAX 6
#define TONE_MIN 1000
#define TONE_MAX 2000
#define TONE_DURATION_DEFAULT 100
#define TONE_DURATION_OFFSET 40
#define TONE_HUNGRY_OFFSET 500
#define TONE_LONELY_OFFSET -900

#define MAX_SATIATION 1000
#define HUNGER_RATE 5000
#define HUNGER_DECAY 2
#define HUNGER_WALK 5

#define FEEDING_VALUE 50

#define MAX_HAPPINESS 1000
#define HAPPINESS_RATE 8000
#define HAPPINESS_DECAY 1

#define PET_TTL 3000

struct VoiceMessage {
    uint16_t *voice;
    uint8_t voiceLength;
    int8_t toneOffset;
};

struct PetLooks {
    uint8_t headId, bodyId;
};

struct PetAttributes {
    uint64_t speakInterval, blinkInterval;
    uint8_t walkRate, voiceLength;
    uint16_t voice[VOICE_LENGTH_MAX * 2];
};

struct PetData {
    int16_t satiation, happiness;
};

typedef struct PetState {
    PetLooks looks;
    PetAttributes attributes;
    PetData data;
} PetState;

class Pet {
    Adafruit_SSD1306 *displayDriver;
    QueueHandle_t voiceQueue;

    uint8_t bodyId, headId, face = FACE_IDLE; // Look index

    Vector2D position, velocity = {0, 0};

    uint64_t blinkLast = 0, blinkInterval;
    bool isBlinking = false;
    
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

    int16_t happiness = MAX_HAPPINESS;
    uint64_t lonelyLast = 0;
    void _happinessReduction(uint16_t value);
    void _happinessCheck();

    int16_t satiation = MAX_SATIATION;
    uint64_t hungerLast = 0;
    void _satiationReduction(uint16_t value);
    void _satiationCheck();
    bool hungry = false;

    void _faceCheck();
    
    bool isHighlighted = false;
    
    public:
        Pet(Adafruit_SSD1306 *display, QueueHandle_t voiceQueue);
        
        void update();
        void draw();

        PetLooks getLooks();
        PetData getData();
        PetAttributes getAttributes();
        
        void setLooks(uint8_t body, uint8_t head);
        void setAttributes(uint64_t speakInterval, uint64_t blinkInterval, uint8_t walkRate, uint8_t voiceLength, uint16_t voice[]);
        void setData(int16_t satiation, int16_t happiness);

        void speak(int16_t toneOffset = 0);

        void feed(uint8_t value);

        uint8_t getFace();

        bool isHungry();

        void setHighlight(bool isHighlighted);
        void toggleHighlight();

    static Vector2D GetRandomPosition();
};



#endif