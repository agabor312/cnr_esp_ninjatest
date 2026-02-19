#ifndef QUEUE_H
#define QUEUE_H

#include <deque>
#include <vector>
#include <cstring>
#include <mutex>

// We need a real structure to hold the queue data for testing
struct MockQueue {
    std::deque<std::vector<uint8_t>> items;
    size_t item_size;
    size_t max_items;
    std::mutex mtx;
};

typedef MockQueue* QueueHandle_t;
typedef int BaseType_t;

#define pdTRUE 1
#define pdFALSE 0

static QueueHandle_t xQueueCreate(size_t uxQueueLength, size_t uxItemSize) {
    MockQueue* q = new MockQueue();
    q->item_size = uxItemSize;
    q->max_items = uxQueueLength;
    return q;
}

static BaseType_t xQueueSend(QueueHandle_t xQueue, const void * pvItemToQueue, TickType_t xTicksToWait) {
    if (!xQueue) return pdFALSE;
    std::lock_guard<std::mutex> lock(xQueue->mtx);
    
    if (xQueue->items.size() >= xQueue->max_items) return pdFALSE; // Full

    // Copy data
    std::vector<uint8_t> data(xQueue->item_size);
    std::memcpy(data.data(), pvItemToQueue, xQueue->item_size);
    xQueue->items.push_back(data);
    
    return pdTRUE;
}

static BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait) {
    if (!xQueue) return pdFALSE;
    std::lock_guard<std::mutex> lock(xQueue->mtx);

    if (xQueue->items.empty()) return pdFALSE;

    // Copy data out
    std::memcpy(pvBuffer, xQueue->items.front().data(), xQueue->item_size);
    xQueue->items.pop_front();

    return pdTRUE;
}

static void vQueueDelete(QueueHandle_t xQueue) {
    if (xQueue) delete xQueue;
}

#endif
