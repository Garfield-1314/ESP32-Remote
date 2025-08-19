#include "mac_receiver.h"
#include <Arduino.h>

// 初始化静态成员
MacReceiver::DataCallback MacReceiver::_dataCallback = nullptr;
bool MacReceiver::_connectionEstablished = false;
uint32_t MacReceiver::_lastReceivedTime = 0;
uint32_t MacReceiver::_timeout = 5000; // 默认超时5秒

MacReceiver::MacReceiver() {
    // 构造函数
}

void MacReceiver::begin() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    
    esp_err_t result = esp_wifi_config_espnow_rate(WIFI_IF_STA, WIFI_PHY_RATE_1M_L);
    
    if(result == ESP_OK) {
        Serial.println("ESP-NOW速率已设置为1Mbps");
    } else {
        Serial.printf("速率设置失败: %s\n", esp_err_to_name(result));
    }

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW初始化失败!");
        return;
    }
    
    esp_now_register_recv_cb(_onDataRecv);
    
    Serial.print("接收端MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.println("等待接收数据...");
}

void MacReceiver::update() {
    // 检查连接状态（超时判定）
    if (_connectionEstablished && (millis() - _lastReceivedTime > _timeout)) {
        Serial.println("连接超时，可能已断开!");
        _connectionEstablished = false;
    }
}

void MacReceiver::setCallback(DataCallback callback) {
    _dataCallback = callback;
}

bool MacReceiver::isConnected() const {
    return _connectionEstablished;
}

uint32_t MacReceiver::getLastReceivedTime() const {
    return _lastReceivedTime;
}

void MacReceiver::setTimeout(uint32_t timeout) {
    _timeout = timeout;
}

void MacReceiver::_printMacAddress(const uint8_t *mac) {
    for (int i = 0; i < 6; i++) {
        Serial.print(mac[i], HEX);
        if (i < 5) Serial.print(":");
    }
}

// ESP-NOW接收回调函数
void MacReceiver::_onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
    if (len == sizeof(DataStruct)) {
        DataStruct receivedData;
        memcpy(&receivedData, incomingData, len);
        
        // 更新状态
        _connectionEstablished = true;
        _lastReceivedTime = millis();
        
        // 如果有用户回调函数，则调用
        if (_dataCallback) {
            _dataCallback(receivedData.id, receivedData.dataArray, info->src_addr);
        } else {
            // 默认打印接收到的数据
            Serial.print("发送方MAC: ");
            _printMacAddress(info->src_addr);
            Serial.println();
            
            Serial.print("接收数据: ID=");
            Serial.print(receivedData.id);
            Serial.print(", Array=[");
            for (int i = 0; i < ARRAY_SIZE; i++) {
                Serial.print(receivedData.dataArray[i]);
                if (i < ARRAY_SIZE - 1) Serial.print(", ");
            }
            Serial.println("]");
        }
    }
}


// 自定义数据处理函数
void handleReceivedData(uint8_t id, uint16_t dataArray[5], const uint8_t *macAddr) {
    Serial.print("来自: ");
    for (int i = 0; i < 6; i++) {
        Serial.print(macAddr[i], HEX);
        if (i < 5) Serial.print(":");
    }
    Serial.println();
    
    Serial.print("data1: ");
    Serial.println(dataArray[0]);
    Serial.print("data2: ");
    Serial.println(dataArray[1]);
    Serial.print("data3: ");
    Serial.println(dataArray[2]);
    Serial.print("data4: ");
    Serial.println(dataArray[3]);
    // 在这里添加你的自定义处理逻辑
}