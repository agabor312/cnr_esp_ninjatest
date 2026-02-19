/**
 * @file test_utils.cpp
 * @brief Utils/RAII unit tests
 */

#include <unity.h>
#include "utils.h"
#include "test_config.h"
#include "test_utils.h"

typedef RAII<int> IntRAII;
typedef RAII<float> FloatRAII;
typedef RAII<Position> PositionRAII;

static IntRAII* intRaii = nullptr;

static void utilsSetUp(void) {
    intRaii = new IntRAII();
}

static void utilsTearDown(void) {
    delete intRaii;
    intRaii = nullptr;
}

static void test_init_creates_mutex(void) {
    bool success = intRaii->init(42);
    TEST_ASSERT_TRUE(success);
}

static void test_lock_modifies_value(void) {
    intRaii->init(100);
    {
        auto lock = intRaii->lock();
        *lock = 200;
    }
    auto lock2 = intRaii->lock();
    TEST_ASSERT_EQUAL(200, *lock2);
}

static void test_auto_unlock(void) {
    intRaii->init(1);
    {
        auto lock = intRaii->lock();
        *lock = 2;
    }
    auto lock = intRaii->lock();
    TEST_ASSERT_EQUAL(2, *lock);
}

void runUtilsTests(void) {
    utilsSetUp();
    RUN_TEST(test_init_creates_mutex);
    utilsTearDown();
    
    utilsSetUp();
    RUN_TEST(test_lock_modifies_value);
    utilsTearDown();
    
    utilsSetUp();
    RUN_TEST(test_auto_unlock);
    utilsTearDown();
}
