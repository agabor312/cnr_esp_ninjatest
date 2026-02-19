#ifndef TASK_H
#define TASK_H

#include <stdint.h>

typedef void (*TaskFunction_t)( void * );
typedef void* TaskHandle_t;
typedef uint32_t TickType_t;

#define pdMS_TO_TICKS(x) (x)

static inline void xTaskCreatePinnedToCore(TaskFunction_t pvTaskCode, const char * const pcName, const uint32_t usStackDepth, void * const pvParameters, uint32_t uxPriority, TaskHandle_t * const pxCreatedTask, const int xCoreID) {
    // In a real test, we might want to actually run this function in a std::thread?
    // For now, let's just ignore it or run it once if needed.
    // Running it once might block if it has a while(1) loop.
    // So better to just do nothing for now, assuming unit tests call run() manually if needed.
}

static inline void vTaskDelay(const TickType_t xTicksToDelay) {
    // No-op or sleep?
}

static inline TickType_t xTaskGetTickCount(void) {
    return 0; // TODO: Hook into mock millis?
}

static inline void vTaskDelayUntil(TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement) {
    *pxPreviousWakeTime += xTimeIncrement;
}

static inline void vTaskDelete(TaskHandle_t xTaskToDelete) {}

#endif
