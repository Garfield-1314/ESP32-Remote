#ifndef MAC_RECEIVER_H
#define MAC_RECEIVER_H

#include <WiFi.h>
#include <esp_now.h>

// 定义数据结构（收发共用）
#define ARRAY_SIZE 10
struct DataStruct {
    uint8_t id;
    uint16_t dataArray[ARRAY_SIZE];
};

class MacTransceiver {
private:
    // 成员变量
    uint8_t _receiverMac[6] = {0xE8, 0x06, 0x90, 0x9B, 0x5A, 0x94};
    bool _sendConnection = false;
    bool _receiveConnection = false;
    uint32_t _lastSendHandshake = 0;
    uint32_t _lastReceiveTime = 0;
    uint32_t _timeout = 3000;
    DataStruct _txData;
    DataStruct _rxData;
    
    static MacTransceiver* _instance; // 静态单例指针
    
    // 私有回调处理器
    void handleDataSent(const uint8_t* mac, esp_now_send_status_t status);
    void handleDataRecv(const uint8_t* senderMac, const uint8_t* data, int len);

public:
    // 构造函数与初始化
    MacTransceiver(uint8_t id = 1);
    void begin(const uint8_t* targetMac = nullptr);
    
    // 发送功能
    void sendData(uint16_t d1, uint16_t d2, uint16_t d3, uint16_t d4, uint16_t d5, uint16_t d6, uint16_t d7, uint16_t d8, uint16_t d9, uint16_t d10);
    void updateSender();
    
    // 接收功能
    void updateReceiver();
    void printReceivedData() const;
    
    // 状态查询
    bool isSendConnected() const { return _sendConnection; }
    bool isReceiveConnected() const { return _receiveConnection; }
    
    // 静态回调包装器（修改后的签名）
    static void OnDataSent(const uint8_t* mac, esp_now_send_status_t status);
    static void OnDataRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len);
};

#endif