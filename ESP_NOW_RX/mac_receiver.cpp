#include "mac_receiver.h"

// 全局SBUS数据数组
int16_t sbusData[10] = {0};

// 初始化静态成员
MacTransceiver* MacTransceiver::_instance = nullptr;

// === 构造函数与初始化 ===
MacTransceiver::MacTransceiver(uint8_t id) {
    _txData.id = id;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        _txData.dataArray[i] = 0;
        _rxData.dataArray[i] = 0;
    }
    _instance = this;  // 设置单例实例
}

void MacTransceiver::begin(const uint8_t* targetMac) {
    if(targetMac) memcpy(_receiverMac, targetMac, 6);
    
    WiFi.mode(WIFI_STA);
    // 设置发送功率
    #ifdef WIFI_POWER_21dBm
    WiFi.setTxPower(WIFI_POWER_21dBm);
    #endif

    // 设置1Mbps传输速率
    esp_wifi_config_espnow_rate(WIFI_IF_STA, WIFI_PHY_RATE_1M_L);
    
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW initialization failed!");
        return;
    }

    // 注册收发回调
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
    
    // 添加对等设备
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, _receiverMac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer device!");
        return;
    }
    else{
        Serial.println("Peer device added successfully!");
    }
    
    Serial.print("Local MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.println("Full-duplex mode ready");
}

// === 发送功能 ===
void MacTransceiver::sendData(uint16_t d1, uint16_t d2, uint16_t d3, uint16_t d4,
                                uint16_t d5,uint16_t d6,uint16_t d7,uint16_t d8,
                                uint16_t d9,uint16_t d10) {
    if (!_sendConnection) return;
    
    _txData.dataArray[0] = d1;
    _txData.dataArray[1] = d2;
    _txData.dataArray[2] = d3;
    _txData.dataArray[3] = d4;
    _txData.dataArray[4] = d5;
    _txData.dataArray[5] = d6;
    _txData.dataArray[6] = d7;
    _txData.dataArray[7] = d8;
    _txData.dataArray[8] = d9;
    _txData.dataArray[9] = d10;
    
    esp_now_send(_receiverMac, (uint8_t*)&_txData, sizeof(_txData));
}

void MacTransceiver::updateSender() {
    // 3秒心跳维持连接
    if (millis() - _lastSendHandshake > 3000) {
        esp_now_send(_receiverMac, (uint8_t*)&_txData, sizeof(_txData));
        _lastSendHandshake = millis();
    }
}

// === 接收功能 ===
void MacTransceiver::updateReceiver() {
    // 接收超时检测
    if (_receiveConnection && (millis() - _lastReceiveTime > _timeout)) {
        Serial.println("Receive timeout, connection may be interrupted!");
        _receiveConnection = false;
    }
}

void MacTransceiver::printReceivedData() const {
    // Serial.print("Received data [ID:");
    // Serial.print(_rxData.id);
    // Serial.print("]: ");
        for(int i = 0; i < 10; i++) {
            // 将0-2047缩放到SBUS标准范围172-1811
            sbusData[i] = (_rxData.dataArray[i] * (1811 - 172)) / 2047 + 172;
            // Serial.print(sbusData[i]);
            // if(i < 9) Serial.print(",");
        }
    // Serial.println();
}

// === 回调处理器 ===
void MacTransceiver::handleDataSent(const uint8_t* mac, esp_now_send_status_t status) {
    if (memcmp(mac, _receiverMac, 6) == 0) {
        _sendConnection = (status == ESP_NOW_SEND_SUCCESS);
    }
}

void MacTransceiver::handleDataRecv(const uint8_t* senderMac, const uint8_t* data, int len) {
    if (len == sizeof(DataStruct)) {
        memcpy(&_rxData, data, len);
        _receiveConnection = true;
        _lastReceiveTime = millis();
    }
}

// === 静态回调包装器 ===
void MacTransceiver::OnDataSent(const uint8_t* mac, esp_now_send_status_t status) {
    if (_instance) {
        _instance->handleDataSent(mac, status);
    }
}

void MacTransceiver::OnDataRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
    if (_instance && info != nullptr) {
        _instance->handleDataRecv(info->src_addr, data, len);
    }
}



