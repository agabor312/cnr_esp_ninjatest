#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <freertos/semphr.h>

// T is the type of data you want to protect (e.g., RobotPose, ServoFeedback, int, float)
template <typename T>
class RAII {
private:
    T data;
    SemaphoreHandle_t mutex;

public:
    // Explicit Init (Embedded Friendly)
    // Returns true if mutex creation succeeded
    bool init(T initialValue = T()) {
        mutex = xSemaphoreCreateMutex();
        data = initialValue;
        return (mutex != NULL);
    }

    // --- The RAII "Locker" Class ---
    class Lock {
    private:
        T* ptr;
        SemaphoreHandle_t m;

    public:
        // Constructor: Takes the Mutex
        Lock(T* p, SemaphoreHandle_t mu) : ptr(p), m(mu) {
            if (m) xSemaphoreTake(m, portMAX_DELAY);
        }
        // Destructor: Releases the Mutex automatically
        ~Lock() {
            if (m) xSemaphoreGive(m);
        }
        // Overloaded Arrow Operator ->
        // This allows you to access members of T directly (e.g., lock->x)
        T* operator->() { return ptr; }

        // Overloaded Dereference Operator *
        // Allows access to the whole object (e.g., *lock = newValue)
        T& operator*() { return *ptr; }
    };
    // Function to get the lock
    Lock lock() {
        return Lock(&data, mutex);
    }
    // Disable copying to prevent two objects sharing the same raw mutex handle
    RAII(const RAII&) = delete;
    RAII& operator=(const RAII&) = delete;
    RAII() : mutex(NULL) {}
};

#endif
