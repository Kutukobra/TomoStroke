#ifndef MESH_CONTROLLER_H
#define MESH_CONTROLLER_H

#include <Arduino.h>
#include <painlessMesh.h>
#include "esp_system.h"
#include <Pet.hpp>

#define MESH_PREFIX   "TomoStroke"
#define MESH_PASSWORD "Strokingonit"
#define MESH_PORT     5555

struct PetPacket {
    String mac;
    uint64_t ttl = 500;

    PetState state;
};

class MeshController {
    painlessMesh mesh;
    QueueHandle_t petPacketQueue; // Output
    QueueHandle_t friendFeedQueue; // Output 

    public:
        MeshController(QueueHandle_t petDataQueue, QueueHandle_t friendFeedQueue);
        void setup();
        void broadcast(PetState pet);
        void feedFriend(const String& targetMac);
        void receivedCallback(uint32_t from, String &msg);
        
    static String GetOwnMac();
};


#endif
