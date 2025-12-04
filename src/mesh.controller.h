#ifndef MESH_CONTROLLER_H
#define MESH_CONTROLLER_H

#include <Arduino.h>
#include <vector>

struct PeerPet {
    String mac;
    uint8_t bodyId;
    uint8_t headId;
    uint32_t blink;
    uint8_t walkRate;
    uint8_t voiceLen;
    uint8_t hunger;
    uint8_t social;
    unsigned long lastSeen;
};

extern std::vector<PeerPet> peers;

void meshSetup();
void meshLoop();
void taskBroadcast();
void taskFeedFriend(const String& targetMac);

#endif
