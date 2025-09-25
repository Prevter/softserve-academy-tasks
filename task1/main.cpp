#include <cstdio>
#include <string>
#include <gtest/gtest.h>
#include "StringList.hpp"

// Test basic creation and destruction
TEST(StringListTest, CreateAndDestroy) {
    char** list = StringListCreate();
    EXPECT_NE(list, nullptr);
    EXPECT_EQ(StringListSize(list), 0);

    StringListDestroy(&list);
    EXPECT_EQ(list, nullptr);
}

// Test adding strings
TEST(StringListTest, AddStrings) {
    char** list = StringListCreate();

    StringListAdd(&list, "First");
    EXPECT_EQ(StringListSize(list), 1);
    EXPECT_STREQ(list[0], "First");

    StringListAdd(&list, "Second");
    EXPECT_EQ(StringListSize(list), 2);
    EXPECT_STREQ(list[0], "First");
    EXPECT_STREQ(list[1], "Second");

    // Test adding empty string
    StringListAdd(&list, "");
    EXPECT_EQ(StringListSize(list), 3);
    EXPECT_STREQ(list[2], "");

    // Test adding string with special characters
    StringListAdd(&list, "Special!@#$%^&*()");
    EXPECT_EQ(StringListSize(list), 4);
    EXPECT_STREQ(list[3], "Special!@#$%^&*()");

    StringListDestroy(&list);
}

// Test indexOf functionality
TEST(StringListTest, IndexOf) {
    char** list = StringListCreate();

    StringListAdd(&list, "Apple");
    StringListAdd(&list, "Banana");
    StringListAdd(&list, "Cherry");
    StringListAdd(&list, "Apple"); // Duplicate

    EXPECT_EQ(StringListIndexOf(list, "Apple"), 0);    // First occurrence
    EXPECT_EQ(StringListIndexOf(list, "Banana"), 1);
    EXPECT_EQ(StringListIndexOf(list, "Cherry"), 2);
    EXPECT_EQ(StringListIndexOf(list, "NotFound"), -1);
    EXPECT_EQ(StringListIndexOf(list, ""), -1);        // Empty string not in list

    // Test case sensitivity
    EXPECT_EQ(StringListIndexOf(list, "apple"), -1);   // Different case

    StringListDestroy(&list);
}

// Test removing by index
TEST(StringListTest, RemoveAt) {
    char** list = StringListCreate();

    StringListAdd(&list, "First");
    StringListAdd(&list, "Second");
    StringListAdd(&list, "Third");
    StringListAdd(&list, "Fourth");

    // Remove from middle
    StringListRemoveAt(list, 1);
    EXPECT_EQ(StringListSize(list), 3);
    EXPECT_STREQ(list[0], "First");
    EXPECT_STREQ(list[1], "Third");
    EXPECT_STREQ(list[2], "Fourth");

    // Remove from beginning
    StringListRemoveAt(list, 0);
    EXPECT_EQ(StringListSize(list), 2);
    EXPECT_STREQ(list[0], "Third");
    EXPECT_STREQ(list[1], "Fourth");

    // Remove from end
    StringListRemoveAt(list, 1);
    EXPECT_EQ(StringListSize(list), 1);
    EXPECT_STREQ(list[0], "Third");

    StringListDestroy(&list);
}

// Test removing by value
TEST(StringListTest, Remove) {
    char** list = StringListCreate();

    StringListAdd(&list, "Apple");
    StringListAdd(&list, "Banana");
    StringListAdd(&list, "Cherry");
    StringListAdd(&list, "Apple"); // Duplicate

    // Remove first occurrence
    StringListRemove(list, "Apple");
    EXPECT_EQ(StringListSize(list), 3);
    EXPECT_STREQ(list[0], "Banana");
    EXPECT_STREQ(list[1], "Cherry");
    EXPECT_STREQ(list[2], "Apple"); // Second Apple should remain

    // Try to remove non-existent string
    size_t sizeBefore = StringListSize(list);
    StringListRemove(list, "NotFound");
    EXPECT_EQ(StringListSize(list), sizeBefore); // Size should remain unchanged

    StringListDestroy(&list);
}

// Test removing duplicates
TEST(StringListTest, RemoveDuplicates) {
    char** list = StringListCreate();

    StringListAdd(&list, "Apple");
    StringListAdd(&list, "Banana");
    StringListAdd(&list, "Apple");
    StringListAdd(&list, "Cherry");
    StringListAdd(&list, "Banana");
    StringListAdd(&list, "Apple");

    StringListRemoveDuplicates(list);
    EXPECT_EQ(StringListSize(list), 3);
    EXPECT_STREQ(list[0], "Apple");
    EXPECT_STREQ(list[1], "Banana");
    EXPECT_STREQ(list[2], "Cherry");

    // Test with no duplicates
    StringListDestroy(&list);
    list = StringListCreate();
    StringListAdd(&list, "Unique1");
    StringListAdd(&list, "Unique2");
    StringListAdd(&list, "Unique3");

    StringListRemoveDuplicates(list);
    EXPECT_EQ(StringListSize(list), 3);

    // Test with empty list
    StringListDestroy(&list);
    list = StringListCreate();
    StringListRemoveDuplicates(list);
    EXPECT_EQ(StringListSize(list), 0);

    StringListDestroy(&list);
}

// Test string replacement
TEST(StringListTest, ReplaceInStrings) {
    char** list = StringListCreate();

    StringListAdd(&list, "Hello World");
    StringListAdd(&list, "Hello Universe");
    StringListAdd(&list, "Goodbye World");

    // Replace "Hello" with "Hi"
    StringListReplaceInStrings(list, "Hello", "Hi");
    EXPECT_STREQ(list[0], "Hi World");
    EXPECT_STREQ(list[1], "Hi Universe");
    EXPECT_STREQ(list[2], "Goodbye World"); // Should remain unchanged

    // Replace "World" with "Earth"
    StringListReplaceInStrings(list, "World", "Earth");
    EXPECT_STREQ(list[0], "Hi Earth");
    EXPECT_STREQ(list[1], "Hi Universe");
    EXPECT_STREQ(list[2], "Goodbye Earth");

    // Test replacement with empty string
    StringListReplaceInStrings(list, "Hi", "");
    EXPECT_STREQ(list[0], " Earth");
    EXPECT_STREQ(list[1], " Universe");

    // Test replacing with longer string
    StringListDestroy(&list);
    list = StringListCreate();
    StringListAdd(&list, "cat");
    StringListReplaceInStrings(list, "cat", "dog");
    EXPECT_STREQ(list[0], "dog");

    StringListDestroy(&list);
}

// Test sorting
TEST(StringListTest, Sort) {
    char** list = StringListCreate();

    StringListAdd(&list, "Zebra");
    StringListAdd(&list, "Apple");
    StringListAdd(&list, "Monkey");
    StringListAdd(&list, "Banana");

    StringListSort(list);
    EXPECT_STREQ(list[0], "Apple");
    EXPECT_STREQ(list[1], "Banana");
    EXPECT_STREQ(list[2], "Monkey");
    EXPECT_STREQ(list[3], "Zebra");

    // Test sorting with numbers and special characters
    StringListDestroy(&list);
    list = StringListCreate();
    StringListAdd(&list, "3");
    StringListAdd(&list, "1");
    StringListAdd(&list, "2");
    StringListAdd(&list, "10");

    StringListSort(list);
    EXPECT_STREQ(list[0], "1");
    EXPECT_STREQ(list[1], "10"); // Lexicographical order
    EXPECT_STREQ(list[2], "2");
    EXPECT_STREQ(list[3], "3");

    // Test sorting empty list
    StringListDestroy(&list);
    list = StringListCreate();
    StringListSort(list);
    EXPECT_EQ(StringListSize(list), 0);

    // Test sorting single element
    StringListAdd(&list, "Only");
    StringListSort(list);
    EXPECT_EQ(StringListSize(list), 1);
    EXPECT_STREQ(list[0], "Only");

    StringListDestroy(&list);
}

// Test edge cases
TEST(StringListTest, EdgeCases) {
    char** list = StringListCreate();

    // Test with very long string
    std::string longString(1000, 'A');
    StringListAdd(&list, longString.c_str());
    EXPECT_EQ(StringListSize(list), 1);
    EXPECT_STREQ(list[0], longString.c_str());

    // Test with string containing null characters (up to first null)
    StringListAdd(&list, "Test\0Hidden");
    EXPECT_EQ(StringListSize(list), 2);
    EXPECT_STREQ(list[1], "Test");

    StringListDestroy(&list);
}

TEST(StringListTest, BasicOperations) {
    char** mylist = StringListCreate();

    StringListAdd(&mylist, "World");
    StringListAdd(&mylist, "Hello");
    StringListAdd(&mylist, "World");
    StringListAdd(&mylist, "HeHeHeheheHe");
    StringListAdd(&mylist, "B");
    StringListAdd(&mylist, "A");
    StringListAdd(&mylist, "C");
    StringListAdd(&mylist, "J");
    StringListAdd(&mylist, "R");
    StringListAdd(&mylist, "D");

    EXPECT_EQ(StringListSize(mylist), 10);

    EXPECT_EQ(StringListIndexOf(mylist, "Hello"), 1);
    EXPECT_EQ(StringListIndexOf(mylist, "NonExistent"), -1);

    StringListRemove(mylist, "Hello");
    EXPECT_EQ(StringListSize(mylist), 9);
    EXPECT_EQ(StringListIndexOf(mylist, "Hello"), -1);

    StringListRemoveDuplicates(mylist);
    EXPECT_EQ(StringListSize(mylist), 8);
    EXPECT_EQ(StringListIndexOf(mylist, "World"), 0);

    StringListReplaceInStrings(mylist, "He", "Ha ");
    EXPECT_EQ(StringListIndexOf(mylist, "Ha Ha Ha heheHa "), 1);

    StringListSort(mylist);
    EXPECT_STREQ(mylist[0], "A");
    EXPECT_STREQ(mylist[1], "B");
    EXPECT_STREQ(mylist[2], "C");

    StringListDestroy(&mylist);
    EXPECT_EQ(mylist, nullptr);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
