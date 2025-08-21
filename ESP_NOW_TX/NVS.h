#ifndef NVS_H
#define NVS_H

#include <Preferences.h>
#include <WString.h>
#include <cstring>

class NVS {
public:
    // 构造函数和析构函数
    NVS();
    ~NVS();
    
    // 初始化与关闭
    void begin(const char* namespace_name = nullptr);
    void end();
    
    // 数据存储方法
    void putInt(const char* key, int value);
    void putLong(const char* key, long value);
    void putFloat(const char* key, float value);
    void putDouble(const char* key, double value);
    void putBool(const char* key, bool value);
    void putString(const char* key, const char* value);
    void putBytes(const char* key, const void* data, size_t len);
    
    // 数据读取方法
    int getInt(const char* key, int default_value = 0);
    long getLong(const char* key, long default_value = 0);
    float getFloat(const char* key, float default_value = 0.0f);
    double getDouble(const char* key, double default_value = 0.0);
    bool getBool(const char* key, bool default_value = false);
    String getString(const char* key, const char* default_value = "");
    size_t getBytes(const char* key, void* buf, size_t buf_len);
    
    // 数据管理
    void remove(const char* key);
    void clear();
    
    // 状态检查
    bool isOpen() const { return _is_open; }
    const char* getNamespace() const { return _namespace; }

private:
    Preferences _prefs;         // ESP32/ESP8266的Preferences对象
    bool _is_open = false;       // 命名空间打开状态
    char _namespace[32] = "";   // 命名空间名称
};

#endif // NVS_H