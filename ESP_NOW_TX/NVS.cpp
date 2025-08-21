#include "NVS.h"

// 构造函数实现
NVS::NVS() 
  : _is_open(false)  // 初始化打开状态为 false
{
    _namespace[0] = '\0';  // 确保命名空间字符串为空
}

// 析构函数实现
NVS::~NVS() {
    if (_is_open) {
        _prefs.end();  // 确保关闭 Preferences
        _is_open = false;
        // 可选的调试输出
        // Serial.println("NVS 命名空间已自动关闭");
    }
}

void NVS::begin(const char* namespace_name) {
  if (namespace_name != nullptr) {
    strncpy(_namespace, namespace_name, sizeof(_namespace) - 1);
    _namespace[sizeof(_namespace) - 1] = '\0';  // 确保以空字符结尾
  } else {
    _namespace[0] = '\0';  // 默认为空命名空间
  }
  
  if (!_is_open) {
    // 以读写模式打开命名空间
    if(_prefs.begin(_namespace, false)) {
      _is_open = true;
      // 可选的调试输出
      // Serial.printf("NVS 命名空间已打开: %s\n", _namespace);
    } else {
      // 打开失败处理
      // Serial.printf("错误: 无法打开命名空间 %s\n", _namespace);
    }
  }
}

void NVS::end() {
  if (_is_open) {
    _prefs.end();
    _is_open = false;
    // 可选的调试输出
    // Serial.printf("NVS 命名空间已关闭: %s\n", _namespace);
  }
}

// 存储实现 ==================================
void NVS::putInt(const char* key, int value) {
  if (_is_open) _prefs.putInt(key, value);
}

void NVS::putLong(const char* key, long value) {
  if (_is_open) _prefs.putLong(key, value);
}

void NVS::putFloat(const char* key, float value) {
  if (_is_open) _prefs.putFloat(key, value);
}

void NVS::putDouble(const char* key, double value) {
  if (_is_open) _prefs.putDouble(key, value);
}

void NVS::putBool(const char* key, bool value) {
  if (_is_open) _prefs.putBool(key, value);
}

void NVS::putString(const char* key, const char* value) {
  if (_is_open) _prefs.putString(key, value);
}

void NVS::putBytes(const char* key, const void* data, size_t len) {
  if (_is_open) _prefs.putBytes(key, data, len);
}

// 读取实现 ==================================
int NVS::getInt(const char* key, int default_value) {
  return _is_open ? _prefs.getInt(key, default_value) : default_value;
}

long NVS::getLong(const char* key, long default_value) {
  return _is_open ? _prefs.getLong(key, default_value) : default_value;
}

float NVS::getFloat(const char* key, float default_value) {
  return _is_open ? _prefs.getFloat(key, default_value) : default_value;
}

double NVS::getDouble(const char* key, double default_value) {
  return _is_open ? _prefs.getDouble(key, default_value) : default_value;
}

bool NVS::getBool(const char* key, bool default_value) {
  return _is_open ? _prefs.getBool(key, default_value) : default_value;
}

String NVS::getString(const char* key, const char* default_value) {
  return _is_open ? _prefs.getString(key, default_value) : String(default_value);
}

size_t NVS::getBytes(const char* key, void* buf, size_t buf_len) {
  return _is_open ? _prefs.getBytes(key, buf, buf_len) : 0;
}

// 数据管理 ==================================
void NVS::remove(const char* key) {
  if (_is_open) _prefs.remove(key);
}

void NVS::clear() {
  if (_is_open) _prefs.clear();
}