#include "mac_sender.h"

// 静态成员初始化
// const uint8_t MacSender::defaultMac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8_t MacSender::defaultMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
MacSender* MacSender::_instance = nullptr;

// 构造函数
MacSender::MacSender(HardwareSerial &serialPort, 
                     uint8_t rxPin, 
                     uint8_t txPin,
                     const uint8_t* mac) : 
    _serial(serialPort),
    _rxPin(rxPin),
    _txPin(txPin),
    _connectionEstablished(false),
    _lastHandshakeTime(0)
{
    memcpy(_receiverMac, mac, 6);
    
    // 初始化数据结构
    _myData.id = 1;
    for (int i = 0; i < 5; i++) {
        _myData.dataArray[i] = 0;
    }
    
    _instance = this;  // 设置单例实例
}

// 初始化ESP-NOW
void MacSender::begin() {
    _serial.begin(115200);
    HardwareSerial serialPort(1);
    serialPort.begin(115200, SERIAL_8N1, _rxPin, _txPin);
    
    WiFi.mode(WIFI_STA);

    esp_err_t result = esp_wifi_config_espnow_rate(WIFI_IF_STA, WIFI_PHY_RATE_1M_L);
    
    if(result == ESP_OK) {
        serialPort.println("ESP-NOW速率已设置为1Mbps");
    } else {
        serialPort.printf("速率设置失败: %s\n", esp_err_to_name(result));
    }

    if (esp_now_init() != ESP_OK) {
        serialPort.println("ESP-NOW初始化失败!");
        return;
    }

    esp_now_register_send_cb(OnDataSent);
    
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, _receiverMac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        serialPort.println("添加对等设备失败!");
        return;
    }
    
    serialPort.print("发送端MAC: ");
    serialPort.println(WiFi.macAddress());
    serialPort.println("ESP-NOW初始化成功");
    
    delay(2000);
}

// 数据发送回调
void MacSender::OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (_instance) {
        if (status == ESP_NOW_SEND_SUCCESS) {
            _instance->_connectionEstablished = true;
        } else {
            _instance->_connectionEstablished = false;
        }
    }
}

// 发送用户数据
void MacSender::sendData(uint16_t userdata1, uint16_t userdata2, 
                         uint16_t userdata3, uint16_t userdata4) {
    if (!_connectionEstablished) return;
    
    // 更新数据结构
    _myData.dataArray[0] = userdata1;
    _myData.dataArray[1] = userdata2;
    _myData.dataArray[2] = userdata3;
    _myData.dataArray[3] = userdata4;
    
    // 发送数据
    esp_err_t result = esp_now_send(_receiverMac, (uint8_t *)&_myData, sizeof(_myData));
    
    // 记录发送状态
    if (result == ESP_OK) {
        _serial.print("数据发送请求已提交: [");
        _serial.print(_myData.dataArray[0]); _serial.print(",");
        _serial.print(_myData.dataArray[1]); _serial.print(",");
        _serial.print(_myData.dataArray[2]); _serial.print(",");
        _serial.print(_myData.dataArray[3]); _serial.println("]");
    } else {
        _serial.println("发送请求失败!");
    }
}

// 更新连接状态
void MacSender::update() {
    if (millis() - _lastHandshakeTime > 3000) {
        if (!_connectionEstablished) {
            _serial.println("尝试建立连接...");
        }
        
        esp_now_send(_receiverMac, (uint8_t *)&_myData, sizeof(_myData));
        _lastHandshakeTime = millis();
    }
}

// 设置接收方MAC地址
void MacSender::setReceiverMac(const uint8_t* mac) {
    memcpy(_receiverMac, mac, 6);
}

// 检查连接状态
bool MacSender::isConnected() const {
    return _connectionEstablished;
}