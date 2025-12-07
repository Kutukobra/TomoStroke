#include <Arduino.h>
#include <painlessMesh.h>
#include <vector>
#include "esp_system.h"
#include "mesh.controller.h"

#define MESH_PREFIX   "TomoMesh"
#define MESH_PASSWORD "12345678"
#define MESH_PORT     5555

painlessMesh mesh;

#define BROADCAST_INTERVAL  15000
#define PRUNE_INTERVAL  5000
#define PEER_TTL  60000

unsigned long lastBroadcast = 0;
unsigned long lastPrune = 0;

String messageQueue = "";

struct PeerPet {
    String mac;
    uint8_t bodyId = 0;
    uint8_t headId = 0;
    uint32_t blink = 0;
    uint8_t walkRate = 0;
    uint8_t voiceLen = 0;
    uint8_t hunger = 100;
    uint8_t social = 100;
    unsigned long lastSeen = 0;
};

std::vector<PeerPet> peers;

String getOwnMac() {
    uint8_t m[6];
    esp_read_mac(m, ESP_MAC_WIFI_STA);
    char buf[13];
    sprintf(buf, "%02X%02X%02X%02X%02X%02X",
            m[0], m[1], m[2], m[3], m[4], m[5]);
    return String(buf);
}

int findPeer(const String &mac) {
    for (int i = 0; i < peers.size(); i++) {
        if (peers[i].mac == mac)
            return i;
    }
    return -1;
}

void taskBroadcast() {
    if (messageQueue != "") return;

    String mac = getOwnMac();
    uint8_t body = 1;
    uint8_t head = 0;
    uint32_t blink = 2000;
    uint8_t walk = 20;
    uint8_t voiceLen = 10;
    uint8_t hunger = 80;
    uint8_t social = 90;

    String out =
        String("BRD ") +
        mac + " " +
        body + " " +
        head + " " +
        blink + " " +
        walk + " " +
        voiceLen + " " +
        hunger + " " +
        social + ";";

    messageQueue = out;
}

void taskFeedFriend(const String &targetMac) {
    if (messageQueue != "") return;
    String out = "FED " + targetMac + ";";
    messageQueue = out;
}

void processQueue() {
    if (messageQueue == "") return;
    mesh.sendBroadcast(messageQueue);
    messageQueue = "";
}

void receivedCallback(uint32_t from, String &msg) {
    msg.trim();

    if (msg.startsWith("BRD ")) {
        msg.remove(0, 4);
        String mac = msg.substring(0, 12);
        String data = msg.substring(13);
        data.replace(";", "");

        int idx = findPeer(mac);
        PeerPet p;
        if (idx >= 0) p = peers[idx];

        p.mac = mac;

        int ptr = 0;
        p.bodyId = data.substring(ptr).toInt();
        ptr = data.indexOf(' ', ptr) + 1;
        p.headId = data.substring(ptr).toInt();
        ptr = data.indexOf(' ', ptr) + 1;
        p.blink = data.substring(ptr).toInt();
        ptr = data.indexOf(' ', ptr) + 1;
        p.walkRate = data.substring(ptr).toInt();
        ptr = data.indexOf(' ', ptr) + 1;
        p.voiceLen = data.substring(ptr).toInt();
        ptr = data.indexOf(' ', ptr) + 1;
        p.hunger = data.substring(ptr).toInt();
        ptr = data.indexOf(' ', ptr) + 1;
        p.social = data.substring(ptr).toInt();

        p.lastSeen = millis();

        if (idx >= 0)
            peers[idx] = p;
        else
            peers.push_back(p);
    }

    else if (msg.startsWith("FED ")) {
        msg.remove(0, 4);
        msg.replace(";", "");
        String target = msg;
        if (target == getOwnMac()) {
            Serial.println("RECEIVED FEED");
        }
    }
}

void prunePeers() {
    unsigned long now = millis();
    for (int i = peers.size() - 1; i >= 0; i--) {
        if (now - peers[i].lastSeen > PEER_TTL) {
            peers.erase(peers.begin() + i);
        }
    }
}

void meshSetup() {
    mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);
    mesh.onReceive(&receivedCallback);
    lastBroadcast = millis();
    lastPrune = millis();
}

void meshLoop() {
    mesh.update();

    unsigned long now = millis();

    if (now - lastBroadcast >= BROADCAST_INTERVAL) {
        taskBroadcast();
        lastBroadcast = now;
    }

    if (now - lastPrune >= PRUNE_INTERVAL) {
        prunePeers();
        lastPrune = now;
    }

    processQueue();
}
