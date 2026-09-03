#include <unity.h>
#include "TextLayout.h"

void setUp() {}
void tearDown() {}

void test_no_text_returns_no_pages() {
    TextLayout tl("");
    tl.setMetrics(5, 7, 60, 28); // text-size-1: 10x14 area -> 12 chars, 4 lines
    auto pages = tl.pages();
    TEST_ASSERT_EQUAL(0, pages.size());
}

void test_single_short_page() {
    TextLayout tl("The quick");
    tl.setMetrics(5, 7, 60, 28); // 12 chars/line, 4 lines/page
    auto pages = tl.pages();
    TEST_ASSERT_EQUAL(1, pages.size());
    TEST_ASSERT_EQUAL_STRING("The quick", pages[0].c_str());
}

void test_word_wrap() {
    TextLayout tl("Hello world");
    tl.setMetrics(6, 8, 36, 24); // 6 chars/line, 3 lines/page
    auto pages = tl.pages();
    TEST_ASSERT_EQUAL(1, pages.size());
    TEST_ASSERT_EQUAL_STRING("Hello\nworld", pages[0].c_str());
}

void test_long_word_breaks() {
    TextLayout tl("supercalifragilistic");
    tl.setMetrics(6, 8, 30, 32); // 5 chars/line, 4 lines/page
    auto pages = tl.pages();
    TEST_ASSERT_EQUAL(1, pages.size());
    TEST_ASSERT_EQUAL_STRING("super\ncalif\nragil\nistic", pages[0].c_str());
}

void test_pagination() {
    TextLayout tl("one two three");
    tl.setMetrics(6, 8, 30, 16); // 5 chars/line, 2 lines/page
    auto pages = tl.pages();
    TEST_ASSERT_EQUAL(2, pages.size());
    TEST_ASSERT_EQUAL_STRING("one\ntwo", pages[0].c_str());
    TEST_ASSERT_EQUAL_STRING("three", pages[1].c_str());
}

void test_newline_respected() {
    TextLayout tl("line one\nline two");
    tl.setMetrics(6, 8, 80, 40); // plenty of room
    auto pages = tl.pages();
    TEST_ASSERT_EQUAL(1, pages.size());
    TEST_ASSERT_EQUAL_STRING("line one\nline two", pages[0].c_str());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_no_text_returns_no_pages);
    RUN_TEST(test_single_short_page);
    RUN_TEST(test_word_wrap);
    RUN_TEST(test_long_word_breaks);
    RUN_TEST(test_pagination);
    RUN_TEST(test_newline_respected);
    UNITY_END();
    return 0;
}
