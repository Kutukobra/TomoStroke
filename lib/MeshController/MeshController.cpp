#include "MeshController.hpp"

MeshController::MeshController(QueueHandle_t petDataQueue, QueueHandle_t friendFeedQueue) :
    petDataQueue(petDataQueue), friendFeedQueue(friendFeedQueue)
{

}


String MeshController::GetOwnMac() {
    uint8_t m[6];
    esp_read_mac(m, ESP_MAC_WIFI_STA);
    char buf[13];
    sprintf(buf, "%02X%02X%02X%02X%02X%02X",
            m[0], m[1], m[2], m[3], m[4], m[5]);
    return String(buf);
}

void MeshController::broadcast(uint8_t bodyId, uint8_t headId, uint8_t walkRate, uint8_t voiceLen, uint8_t hunger, uint8_t happiness, uint16_t voice[]) {
    String mac = MeshController::GetOwnMac();

    String out =
        String("BRD ") +
        mac + " " +
        bodyId + " " +
        headId + " " +
        walkRate + " " +
        voiceLen + " " +
        hunger + " " +
        happiness + " ";
    
    for (uint8_t i = 0; i < voiceLen * 2; i += 2) {
        out += voice[i] + " ";
        out += voice[i + 1] + " ";
    }

    mesh.sendBroadcast(out);
}

void MeshController::feedFriend(const String &targetMac) {
    String out = "FED " + targetMac + ";";
    mesh.sendBroadcast(out);
}


painlessmesh::receivedCallback_t MeshController::receivedCallback(uint32_t from, String &msg) {
    msg.trim();

    if (msg.startsWith("BRD ")) {
        msg.remove(0, 4);
        String mac = msg.substring(0, 12);
        String data = msg.substring(13);
        data.replace(";", "");

        PetData p;

        p.mac = mac;

        int ptr = 0;
        p.bodyId = data.substring(ptr).toInt();
        ptr = data.indexOf(' ', ptr) + 1;
        p.headId = data.substring(ptr).toInt();
        ptr = data.indexOf(' ', ptr) + 1;
        p.walkRate = data.substring(ptr).toInt();
        ptr = data.indexOf(' ', ptr) + 1;
        p.voiceLen = data.substring(ptr).toInt();
        ptr = data.indexOf(' ', ptr) + 1;
        p.hunger = data.substring(ptr).toInt();
        ptr = data.indexOf(' ', ptr) + 1;
        p.happiness = data.substring(ptr).toInt();

        for (uint8_t i = 0; i < p.voiceLen * 2; i += 2) {
            ptr = data.indexOf(' ', ptr) + 1;
            p.voice[i] = data.substring(ptr).toInt();
            p.voice[i + 1] = data.substring(ptr).toInt();
        }

        p.lastSeen = millis();

        xQueueSend(petDataQueue, &p, 10);
    }

    else if (msg.startsWith("FED ")) {
        msg.remove(0, 4);
        msg.replace(";", "");
        String target = msg;
        if (target == MeshController::GetOwnMac()) {
            Serial.println("RECEIVED FEED");
        }
    }
}

void MeshController::setup() {
    mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);
    mesh.onReceive([this](uint32_t from, String &msg) {
        receivedCallback(from, msg);
    });
}
