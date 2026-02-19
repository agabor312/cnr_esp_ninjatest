#ifndef Arduino_h
#define Arduino_h

#include <stdint.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <cstring>
#include <cstdarg>
#include <cmath>

typedef uint8_t byte;
typedef bool boolean;

// Time Mocks
static unsigned long _mock_millis = 0;
static inline unsigned long millis() { return _mock_millis; }
static inline void delay(unsigned long ms) { _mock_millis += ms; }
static inline unsigned long micros() { return _mock_millis * 1000; }

// Math
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define abs(x) ((x)>0?(x):-(x))
#define round(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

// String Mock (Forward declaration)
class String;

// String Mock (Minimal)
class String {
public:
    std::string _s;
    String(const char* s) : _s(s) {}
    String(std::string s) : _s(s) {}
    String() : _s("") {}

    bool startsWith(const String& prefix) const {
        return _s.rfind(prefix._s, 0) == 0;
    }
    int indexOf(char c) const {
        return _s.find(c);
    }
    String substring(int start, int end = -1) const {
        if (end == -1) return String(_s.substr(start));
        return String(_s.substr(start, end - start));
    }
    float toFloat() const {
        try { return std::stof(_s); } catch(...) { return 0.0f; }
    }
    int toInt() const {
         try { return std::stoi(_s); } catch(...) { return 0; }
    }
    void trim() {
        // simple trim logic
    }
    const char* c_str() const { return _s.c_str(); }
    
    // Operators
    String& operator+=(const String& other) { _s += other._s; return *this; }
    friend String operator+(const String& lhs, const String& rhs) { return String(lhs._s + rhs._s); }
};

// Serial Mock
class HardwareSerial {
public:
    std::stringstream _rx_buffer;
    std::stringstream _tx_buffer;

    void begin(unsigned long baud, uint32_t config=0, int8_t rxPin=-1, int8_t txPin=-1) {}
    
    // Write
    size_t print(const char* s) { 
        _tx_buffer << s; 
        return strlen(s); 
    } 
    size_t println(const char* s) { 
        _tx_buffer << s << "\n"; 
        return strlen(s) + 1; 
    }
    size_t println(int i) {
        _tx_buffer << i << "\n";
        return 1;
    }
    size_t println(float f) {
        _tx_buffer << f << "\n";
        return 1;
    }
    size_t printf(const char *format, ...) {
        char buf[256];
        va_list args;
        va_start(args, format);
        vsnprintf(buf, sizeof(buf), format, args);
        va_end(args);
        _tx_buffer << buf;
        return strlen(buf);
    }

    // Read
    int available() { 
        return _rx_input.length() > 0;
    }
    
    String readStringUntil(char terminator) {
        size_t pos = _rx_input.find(terminator);
        if (pos != std::string::npos) {
            std::string s = _rx_input.substr(0, pos);
            _rx_input.erase(0, pos + 1);
            return String(s.c_str());
        }
        return String("");
    }
    
    void setTimeout(unsigned long timeout) {}

    // Test Helpers
    std::string _rx_input;
    void input(const std::string& s) { _rx_input += s; }
    std::string output() { return _tx_buffer.str(); }
    void clearOutput() { _tx_buffer.str(""); _tx_buffer.clear(); }
};

// Global Serial object
extern HardwareSerial Serial;
extern HardwareSerial Serial2;

// Serial Config Constants
#define SERIAL_8N1 0

#endif
