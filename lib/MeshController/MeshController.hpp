#ifndef MESH_CONTROLLER_H
#define MESH_CONTROLLER_H

#include <Arduino.h>
#include <painlessMesh.h>
#include "esp_system.h"
#include <Pet.hpp>

#define MESH_PREFIX   "TomoStroke"
#define MESH_PASSWORD "Strokingonit"
#define MESH_PORT     5555

#define BROADCAST_INTERVAL  15000
#define PRUNE_INTERVAL  5000
#define PEER_TTL  60000

struct PetData {
    String mac;

    uint8_t bodyId = 0;
    uint8_t headId = 0;
    uint8_t walkRate = 0;
    uint8_t voiceLen = 0;
    uint16_t voice[VOICE_LENGTH_MAX * 2];
    
    int16_t hunger = 100;
    int16_t happiness = 100;

    uint64_t lastSeen = 0;
};

class MeshController {
    painlessMesh mesh;
    QueueHandle_t petDataQueue; // Output
    QueueHandle_t friendFeedQueue; // Output 

    public:
        MeshController(QueueHandle_t petDataQueue, QueueHandle_t friendFeedQueue);
        void setup();
        void broadcast(uint8_t bodyId, uint8_t headId, uint8_t walkRate, uint8_t voiceLen, uint8_t hunger, uint8_t happiness, uint16_t voice[]);
        void feedFriend(const String& targetMac);
        painlessmesh::receivedCallback_t receivedCallback(uint32_t from, String &msg);
        
    static String GetOwnMac();
};


#endif
