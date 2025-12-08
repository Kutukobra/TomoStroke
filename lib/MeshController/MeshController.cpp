#include "MeshController.hpp"

MeshController::MeshController(QueueHandle_t *petDataQueue, QueueHandle_t *friendFeedQueue) :
    petPacketQueue(petDataQueue), friendFeedQueue(friendFeedQueue)
{

}


String MeshController::GetOwnMac() {
    uint8_t m[6];
    if(esp_read_mac(m, ESP_MAC_WIFI_STA) != ESP_OK) {
        return "";
    }

    char buf[13];
    sprintf(buf, "%02X%02X%02X%02X%02X%02X",
            m[0], m[1], m[2], m[3], m[4], m[5]);
    return String(buf);
}

void MeshController::broadcast(PetState pet) {
    String mac = MeshController::GetOwnMac();

    String out =
        String("BRD ") +
        mac + " " +
        pet.looks.headId + " " +
        pet.looks.bodyId + " " +

        pet.attributes.speakInterval + " " +
        pet.attributes.blinkInterval + " " +
        pet.attributes.walkRate + " " +
        pet.attributes.voiceLength + " " +

        pet.data.satiation + " " + 
        pet.data.happiness + " ";
    
    String voice = "";
    for (uint8_t i = 0; i < pet.attributes.voiceLength * 2; i += 2) {
        voice += String(pet.attributes.voice[i]) + " " + String(pet.attributes.voice[i + 1]) + String(" ");
    }

    // Serial.println("Sending: " + out + voice + ";");
    mesh.sendBroadcast(out + voice + ";");
}

void MeshController::feedFriend(const String &targetMac) {
    String out = "FED " + targetMac + ";";
    mesh.sendBroadcast(out);
}


void MeshController::receivedCallback(uint32_t from, String &msg) {
    // Serial.println("Received: " + msg);
    msg.trim();

    if (msg.startsWith("BRD ")) {
        msg.remove(0, 4);
        String mac = msg.substring(0, 12);
        String data = msg.substring(13);
        data.replace(";", "");

        PetPacket p;

        p.mac = mac;
        int ptr = 0;
        p.state.looks.headId = data.substring(ptr).toInt();
        ptr = data.indexOf(' ', ptr) + 1;
        p.state.looks.bodyId = data.substring(ptr).toInt();

        ptr = data.indexOf(' ', ptr) + 1;
        p.state.attributes.speakInterval = data.substring(ptr).toInt();
        ptr = data.indexOf(' ', ptr) + 1;
        p.state.attributes.blinkInterval = data.substring(ptr).toInt();
        ptr = data.indexOf(' ', ptr) + 1;
        p.state.attributes.walkRate = data.substring(ptr).toInt();
        ptr = data.indexOf(' ', ptr) + 1;
        p.state.attributes.voiceLength = data.substring(ptr).toInt();

        ptr = data.indexOf(' ', ptr) + 1;
        p.state.data.satiation = data.substring(ptr).toInt();
        ptr = data.indexOf(' ', ptr) + 1;
        p.state.data.happiness = data.substring(ptr).toInt();

        for (uint8_t i = 0; i < p.state.attributes.voiceLength * 2; i += 2) {
            ptr = data.indexOf(' ', ptr) + 1;
            p.state.attributes.voice[i] = data.substring(ptr).toInt();
            p.state.attributes.voice[i + 1] = data.substring(ptr).toInt();
        }

        xQueueSend(*petPacketQueue, &p, 10);
    }

    else if (msg.startsWith("FED ")) {
        msg.remove(0, 4);
        msg.replace(";", "");
        String target = msg;
        if (target == MeshController::GetOwnMac()) {
            uint8_t feedValue = FEEDING_VALUE;
            xQueueSend(*friendFeedQueue, &feedValue, 10);
        }
    }
}

void MeshController::setup() {
    mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);
    mesh.onReceive([this](uint32_t from, String &msg) {
        receivedCallback(from, msg);
    });
}

void MeshController::update() {
    mesh.update();
}
