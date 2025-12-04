#ifndef SPRITES_HPP
#define SPRITES_HPP

#include <Arduino.h>

#define SPRITE_WIDTH 16
#define SPRITE_HEIGHT 16

#define FACE_WIDTH 8
#define FACE_HEIGHT 4

// Faces
extern const uint8_t sprite_face_idle[] PROGMEM;
extern const uint8_t sprite_face_blink[] PROGMEM;
extern const uint8_t sprite_face_happy[] PROGMEM;
extern const uint8_t sprite_face_sad[] PROGMEM;
extern const uint8_t sprite_face_surprised[] PROGMEM;

#define FACE_COUNT 5
#define FACE_IDLE 0
#define FACE_BLINK 1
#define FACE_HAPPY 2
#define FACE_SAD 3
#define FACE_SURPRISED 4

extern const uint8_t* const sprite_faces[FACE_COUNT];

// Bodies
extern const uint8_t sprite_body_1[] PROGMEM;
extern const uint8_t sprite_body_2[] PROGMEM;
extern const uint8_t sprite_body_3[] PROGMEM;
extern const uint8_t sprite_body_4[] PROGMEM;
extern const uint8_t sprite_body_5[] PROGMEM;
extern const uint8_t sprite_body_6[] PROGMEM;
extern const uint8_t sprite_body_7[] PROGMEM;

#define BODY_COUNT 7
#define BODY_WIDTH 16
#define BODY_HEIGHT 8
extern const uint8_t* const sprite_bodies[BODY_COUNT];

// Heads
extern const uint8_t sprite_head_1[] PROGMEM;
extern const uint8_t sprite_head_2[] PROGMEM;
extern const uint8_t sprite_head_3[] PROGMEM;
extern const uint8_t sprite_head_4[] PROGMEM;
extern const uint8_t sprite_head_5[] PROGMEM;
extern const uint8_t sprite_head_6[] PROGMEM;
extern const uint8_t sprite_head_7[] PROGMEM;
extern const uint8_t sprite_head_8[] PROGMEM;

#define HEAD_COUNT 8
#define HEAD_WIDTH 16
#define HEAD_HEIGHT 8

extern const uint8_t* const sprite_heads[HEAD_COUNT];

// Icon
extern const unsigned char sprite_icon_satiated [] PROGMEM;
extern const unsigned char sprite_icon_hungry [] PROGMEM;
extern const unsigned char sprite_icon_happy [] PROGMEM;
extern const unsigned char sprite_icon_bored [] PROGMEM;
extern const unsigned char sprite_icon_sad [] PROGMEM;

#define ICON_SATIATED 0
#define ICON_HUNGRY 1
#define ICON_HAPPY 2
#define ICON_BORED 3
#define ICON_SAD 4

#endif
