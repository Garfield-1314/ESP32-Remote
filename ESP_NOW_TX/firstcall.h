#ifndef FIRSTCALL_H
#define FIRSTCALL_H

#include <esp_now.h>
#include <WiFi.h>
#include <vector>
#include <array>

class FirstCall {
public:
    FirstCall(uint8_t id);
    void begin();
    void processDiscovery();
    const uint8_t* getPeerMac() const { return _peerMac; }
    bool isConnected() const { return _connected; }

private:
    // 修正1: 修改回调签名以匹配ESP-NOW要求
    static void onReceive(const esp_now_recv_info_t* info, const uint8_t* data, int len);
    static void onSend(const uint8_t* mac, esp_now_send_status_t status);
    
    // 处理接收数据的内部方法
    void handleReceive(const uint8_t* senderMac, const uint8_t* data, int len);
    
    void sendDiscovery();
    void addPeer(const uint8_t* mac);
    void switchToP2P(const uint8_t* peerMac);
    
    static FirstCall* _instance;
    
    uint8_t _selfMac[6]; 
    uint8_t _peerMac[6];
    uint8_t _deviceID;
    bool _connected = false;
    unsigned long _lastSendTime = 0;
    unsigned long _discoverStart = 0;
    
    // 修正2: 使用固定大小数组而不是原始数组
    std::vector<std::array<uint8_t, 6>> _discoveredPeers;
};

#endif