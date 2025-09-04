#include "firstcall.h"
#include <cstring>
#include "NVS.h"
#include "led.h"


const uint8_t BROADCAST_MAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
FirstCall* FirstCall::_instance = nullptr;

FirstCall::FirstCall(uint8_t id) : _deviceID(id) {
    WiFi.macAddress(_selfMac);
    memset(_peerMac, 0, 6);
    _instance = this;
}

void FirstCall::begin() {
    WiFi.mode(WIFI_STA);
    
    if (esp_now_init() != ESP_OK) {
        Serial.println("[FirstCall] ESP-NOW init failed!");
        return;
    }

    // 修正3: 使用正确的回调签名
    esp_now_register_send_cb(onSend);
    esp_now_register_recv_cb(onReceive);
    
    addPeer(BROADCAST_MAC);
    
    Serial.print("[FirstCall] Started. Local MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.println("Waiting for peer discovery...");
    _discoverStart = millis();
}

extern LED led;
void FirstCall::processDiscovery() {
    if (_connected) return;
    
    if (millis() - _lastSendTime > 1000) {
        sendDiscovery();
        led.toggle(); // 闪烁LED表示正在发现
        _lastSendTime = millis();
        
        if (millis() - _discoverStart > 30000) {
            Serial.println("[FirstCall] Discovery timeout. Please reset devices.");
        }
    }
}

void FirstCall::sendDiscovery() {
    uint8_t discoveryData[7];
    memcpy(discoveryData, _selfMac, 6);
    discoveryData[6] = _deviceID;
    
    esp_now_send(BROADCAST_MAC, discoveryData, sizeof(discoveryData));
    Serial.println("[FirstCall] Sending discovery broadcast...");
}

void FirstCall::addPeer(const uint8_t* mac) {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("[FirstCall] Failed to add peer");
    }
}

// 修正4: 静态回调方法通过_instance调用实例方法
void FirstCall::onReceive(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
    if (_instance && info) {
        _instance->handleReceive(info->src_addr, data, len);
    }
}

void FirstCall::onSend(const uint8_t* mac, esp_now_send_status_t status) {
    if (_instance) {
        if (status != ESP_NOW_SEND_SUCCESS && _instance->_connected) {
            Serial.println("[FirstCall] P2P send failed! Reverting to broadcast.");
            _instance->_connected = false;
            _instance->addPeer(BROADCAST_MAC);
        }
    }
}

extern NVS nvs;

void FirstCall::handleReceive(const uint8_t* senderMac, const uint8_t* data, int len) {
    // 检查是否来自自己的消息
    if (memcmp(senderMac, _selfMac, 6) == 0) return;
    
    if (len == 7) {
        const uint8_t senderID = data[6];
        Serial.printf("[FirstCall] Received discovery from %02X:%02X:%02X:%02X:%02X:%02X (ID: %d)\n",
                      senderMac[0], senderMac[1], senderMac[2], 
                      senderMac[3], senderMac[4], senderMac[5],
                      senderID);
                      
        nvs.putInt("MAC0",senderMac[0]);
        nvs.putInt("MAC1",senderMac[1]);
        nvs.putInt("MAC2",senderMac[2]);
        nvs.putInt("MAC3",senderMac[3]);
        nvs.putInt("MAC4",senderMac[4]);
        nvs.putInt("MAC5",senderMac[5]);

        bool alreadyKnown = false;
        for (const auto& peer : _discoveredPeers) {
            if (memcmp(peer.data(), senderMac, 6) == 0) {
                alreadyKnown = true;
                break;
            }
        }
        
        if (!alreadyKnown) {
            std::array<uint8_t, 6> newPeer;
            memcpy(newPeer.data(), senderMac, 6);
            _discoveredPeers.push_back(newPeer);
        }
        
        memcpy(_peerMac, senderMac, 6);
        
        if (!_discoveredPeers.empty()) {
            esp_now_del_peer(BROADCAST_MAC);
            addPeer(_peerMac);
            Serial.printf("[FirstCall] Switching to P2P with: %02X:%02X:%02X:%02X:%02X:%02X\n",
                          _peerMac[0], _peerMac[1], _peerMac[2],
                          _peerMac[3], _peerMac[4], _peerMac[5]);
            
            uint8_t ackData[] = {0xAA};
            esp_now_send(_peerMac, ackData, sizeof(ackData));
            _connected = true;
        }
    }
    else if (len == 1 && data[0] == 0xAA) {
        Serial.println("[FirstCall] Received connection ACK. P2P established!");
        _connected = true;
    }
}