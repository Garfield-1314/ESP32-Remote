#ifndef MAC_RECEIVER_H
#define MAC_RECEIVER_H

#include <esp_now.h>
#include <WiFi.h>

// 必须与发送端保持一致
#define ARRAY_SIZE 5

class MacReceiver {
public:
    // 接收数据的回调函数类型
    typedef void (*DataCallback)(uint8_t id, uint16_t dataArray[ARRAY_SIZE], const uint8_t *macAddr);
    
    MacReceiver();
    
    // 初始化接收器
    void begin();
    
    // 需要在主循环中定期调用
    void update();
    
    // 设置数据接收回调函数
    void setCallback(DataCallback callback);
    
    // 获取连接状态
    bool isConnected() const;
    
    // 获取最后接收时间
    uint32_t getLastReceivedTime() const;
    
    // 设置超时时间（毫秒）
    void setTimeout(uint32_t timeout);

    

private:
    // 与发送端匹配的数据结构
    struct DataStruct {
        uint8_t id;
        uint16_t dataArray[ARRAY_SIZE];
    };
    
    static DataCallback _dataCallback;       // 用户回调函数
    static bool _connectionEstablished;      // 连接状态
    static uint32_t _lastReceivedTime;       // 最后接收时间
    static uint32_t _timeout;                // 超时时间（默认5000ms）
    
    // ESP-NOW接收回调函数（静态成员）
    static void _onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len);
    
    // 打印MAC地址的辅助函数
    static void _printMacAddress(const uint8_t *mac);

    
};

void handleReceivedData(uint8_t id, uint16_t dataArray[ARRAY_SIZE], const uint8_t *macAddr);

#endif