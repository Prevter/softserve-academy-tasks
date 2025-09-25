#include "StringList.hpp"
#include <cstring>

char** StringListCreate() {
    auto list = static_cast<char**>(malloc(sizeof(char*)));
    list[0] = nullptr;
    return list;
}

void StringListDestroy(char*** list) {
    if (list && *list) {
        auto ptr = *list;
        while (ptr && *ptr) {
            free(*ptr);
            ++ptr;
        }
        free(*list);
        *list = nullptr;
    }
}

size_t StringListSize(char* const* list) {
    size_t size = 0;
    while (list[size]) size++;
    return size;
}

void StringListAdd(char*** list, char const* str) {
    auto len = StringListSize(*list);
    auto newList = static_cast<char**>(realloc(*list, sizeof(char*) * (len + 2)));
    *list = newList;
    newList[len] = static_cast<char*>(malloc(strlen(str) + 1));
    strcpy(newList[len], str);
    newList[len + 1] = nullptr;
}

ssize_t StringListIndexOf(char* const* list, char const* str) {
    ssize_t index = 0;
    while (list[index]) {
        if (strcmp(list[index], str) == 0) {
            return index;
        }
        ++index;
    }
    return -1; // not found
}

void StringListRemoveAt(char** list, size_t index) {
    size_t len = StringListSize(list);
    if (index >= len) return; // out of bounds

    free(list[index]);

    for (size_t i = index; i < len - 1; ++i) {
        list[i] = list[i + 1];
    }
    list[len - 1] = nullptr;
}

void StringListRemove(char** list, char const* str) {
    size_t index = StringListIndexOf(list, str);
    if (index == static_cast<size_t>(-1)) return; // not found
    StringListRemoveAt(list, index);
}

void StringListRemoveDuplicates(char** list) {
    size_t len = StringListSize(list);
    for (size_t i = 0; i < len; ++i) {
        for (size_t j = i + 1; j < len; ) {
            if (strcmp(list[i], list[j]) == 0) {
                StringListRemoveAt(list, j);
                --len; // list size decreased
            } else {
                ++j;
            }
        }
    }
}

void StringListReplaceInStrings(char** list, char const* before, char const* after) {
    auto len = StringListSize(list);
    auto beforeLen = strlen(before);
    auto afterLen = strlen(after);
    if (beforeLen == 0) return;

    for (size_t i = 0; i < len; ++i) {
        while (char* pos = strstr(list[i], before)) {
            if (beforeLen >= afterLen) {
                // no need to reallocate
                memmove(pos + afterLen, pos + beforeLen, strlen(pos + beforeLen) + 1);
                memcpy(pos, after, afterLen);
            } else {
                // new size is larger, need to reallocate
                size_t newSize = strlen(list[i]) + (afterLen - beforeLen) + 1;
                auto newStr = static_cast<char*>(malloc(newSize));
                size_t prefixLen = pos - list[i];
                memcpy(newStr, list[i], prefixLen);
                memcpy(newStr + prefixLen, after, afterLen);
                strcpy(newStr + prefixLen + afterLen, pos + beforeLen);
                free(list[i]);
                list[i] = newStr;
            }
        }
    }
}

void StringListSort(char** list) {
    auto len = StringListSize(list);
    if (len <= 1) return;

    for (size_t i = 0; i < len - 1; ++i) {
        bool swapped = false;
        for (size_t j = 0; j < len - i - 1; ++j) {
            if (strcmp(list[j], list[j + 1]) > 0) {
                char* temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
                swapped = true;
            }
        }

        if (!swapped) break;
    }
}