#include <unity.h>
#include <ButtonHandler.h>

ButtonHandler handler(600, 150, 0);

void setUp(void) {
    handler.reset();
}

void tearDown(void) {}

void test_wake_short_press() {
    handler.update(false, false, 0);
    handler.update(true, false, 100);  // press
    handler.update(false, false, 200); // release before long threshold
    TEST_ASSERT_TRUE(handler.hasAction());
    TEST_ASSERT_EQUAL(ButtonAction::WAKE_SHORT, handler.getAction());
    TEST_ASSERT_FALSE(handler.hasAction());
}

void test_io_long_press() {
    handler.update(false, false, 0);
    handler.update(false, true, 0);
    handler.update(false, true, 700); // hold past 600 ms
    TEST_ASSERT_TRUE(handler.hasAction());
    TEST_ASSERT_EQUAL(ButtonAction::IO_LONG, handler.getAction());
    handler.update(false, false, 800); // release
    TEST_ASSERT_FALSE(handler.hasAction()); // no short on release after long
}

void test_both_press() {
    handler.update(false, false, 0);
    handler.update(true, false, 0); // wake first
    handler.update(true, true, 50); // io joins
    handler.update(true, true, 250); // hold past both threshold 150 ms
    TEST_ASSERT_TRUE(handler.hasAction());
    TEST_ASSERT_EQUAL(ButtonAction::BOTH, handler.getAction());
}

void test_quick_both_does_not_emit_short() {
    handler.update(false, false, 0);
    handler.update(true, true, 0);   // both pressed
    handler.update(false, false, 50); // released before both threshold
    TEST_ASSERT_FALSE(handler.hasAction());
}

void test_long_after_both_cancelled() {
    // Press wake, then io joins before long threshold, then both released.
    handler.update(false, false, 0);
    handler.update(true, false, 0);
    handler.update(true, true, 100);
    handler.update(false, false, 200);
    TEST_ASSERT_FALSE(handler.hasAction());

    // Now a clean long press on wake should still work.
    handler.update(true, false, 1000);
    handler.update(true, false, 1700);
    TEST_ASSERT_TRUE(handler.hasAction());
    TEST_ASSERT_EQUAL(ButtonAction::WAKE_LONG, handler.getAction());
}

void test_io_double_tap() {
    ButtonHandler d(600, 150, 300);
    d.update(false, false, 0);
    d.update(false, true, 0);      // first press
    d.update(false, true, 100);    // still down
    d.update(false, false, 100);   // release before long threshold
    d.update(false, false, 150);   // within double-tap window
    d.update(false, true, 150);    // second press
    d.update(false, true, 250);    // still down
    d.update(false, false, 250);   // second release -> double tap
    TEST_ASSERT_TRUE(d.hasAction());
    TEST_ASSERT_EQUAL(ButtonAction::IO_DOUBLE, d.getAction());
    TEST_ASSERT_FALSE(d.hasAction());
}

void test_io_double_tap_window_expires() {
    ButtonHandler d(600, 150, 300);
    d.update(false, false, 0);
    d.update(false, true, 0);
    d.update(false, false, 100);   // first release
    d.update(false, false, 450);   // window expired -> emit single short
    TEST_ASSERT_TRUE(d.hasAction());
    TEST_ASSERT_EQUAL(ButtonAction::IO_SHORT, d.getAction());
    TEST_ASSERT_FALSE(d.hasAction());

    d.update(false, true, 450);    // second press after window
    d.update(false, false, 550);   // second release
    d.update(false, false, 900);   // window expired
    TEST_ASSERT_TRUE(d.hasAction());
    TEST_ASSERT_EQUAL(ButtonAction::IO_SHORT, d.getAction());
}

void test_double_tap_second_press_becomes_long() {
    ButtonHandler d(600, 150, 300);
    d.update(false, false, 0);
    d.update(false, true, 0);
    d.update(false, false, 100);   // first short
    d.update(false, true, 150);    // second press
    d.update(false, true, 900);    // held past long threshold -> long
    TEST_ASSERT_TRUE(d.hasAction());
    TEST_ASSERT_EQUAL(ButtonAction::IO_LONG, d.getAction());
    d.update(false, false, 950);   // release
    TEST_ASSERT_FALSE(d.hasAction());
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    UNITY_BEGIN();
    RUN_TEST(test_wake_short_press);
    RUN_TEST(test_io_long_press);
    RUN_TEST(test_both_press);
    RUN_TEST(test_quick_both_does_not_emit_short);
    RUN_TEST(test_long_after_both_cancelled);
    RUN_TEST(test_io_double_tap);
    RUN_TEST(test_io_double_tap_window_expires);
    RUN_TEST(test_double_tap_second_press_becomes_long);
    UNITY_END();
    return 0;
}
