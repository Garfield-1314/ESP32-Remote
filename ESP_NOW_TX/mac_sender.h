#ifndef MAC_SENDER_H
#define MAC_SENDER_H

#include <esp_now.h>
#include <WiFi.h>
#include <HardwareSerial.h>

class MacSender {
public:
    // 初始化发送器
    MacSender(HardwareSerial &serialPort = Serial, 
              uint8_t rxPin = 36, 
              uint8_t txPin = 35,
              const uint8_t* mac = defaultMac);
    
    // 开始通信
    void begin();
    
    // 发送用户数据
    void sendData(uint16_t userdata1, uint16_t userdata2, 
                 uint16_t userdata3, uint16_t userdata4);
    
    // 需要在主循环中定期调用
    void update();
    
    // 检查连接状态
    bool isConnected() const;
    
    // 设置接收方MAC地址
    void setReceiverMac(const uint8_t* mac);

    static const uint8_t defaultMac[6];

private:
    // 数据结构
    struct struct_message {
        uint8_t id;
        uint16_t dataArray[5];  // 5个元素的整数数组
    };

    // 成员变量
    HardwareSerial& _serial;
    uint8_t _rxPin;
    uint8_t _txPin;
    uint8_t _receiverMac[6];
    struct_message _myData;
    bool _connectionEstablished;
    uint32_t _lastHandshakeTime;

    // ESP-NOW回调函数
    static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
    static MacSender* _instance;
};

#endif