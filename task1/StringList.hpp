#pragma once
#include <cstdlib>

/// @brief Creates an empty string list.
/// @return Pointer to the created string list.
char** StringListCreate();

/// @brief Destroys a string list and frees all associated memory. The pointer to the list is set to nullptr.
/// @param list Pointer to the string list to destroy.
void StringListDestroy(char*** list);

/// @brief Returns the number of strings in the list.
/// @param list The string list.
/// @return The number of strings in the list.
size_t StringListSize(char* const* list);

/// @brief Adds a copy of the given string to the end of the list.
/// @param list Pointer to the string list, which will be modified.
/// @param str The string to add.
void StringListAdd(char*** list, char const* str);

/// @brief Returns the index of the first occurrence of the given string in the list, or -1 if not found.
/// @param list The string list.
/// @param str The string to find.
/// @return The index of the string, or -1 if not found.
ssize_t StringListIndexOf(char* const* list, char const* str);

/// @brief Removes the string at the specified index from the list.
/// @param list The string list.
/// @param index The index of the string to remove.
void StringListRemoveAt(char** list, size_t index);

/// @brief Removes the first occurrence of the given string from the list.
/// @param list The string list.
/// @param str The string to remove.
/// @note If the string is not found, the list remains unchanged.
void StringListRemove(char** list, char const* str);

/// @brief Removes duplicate strings from the list, keeping only the first occurrence of each string.
/// @param list The string list.
void StringListRemoveDuplicates(char** list);

/// @brief Replaces all occurrences of a substring in each string of the list with another substring.
/// @param list The string list.
/// @param before The substring to be replaced.
/// @param after The substring to replace with.
void StringListReplaceInStrings(char** list, char const* before, char const* after);

/// @brief Sorts the strings in the list in ascending lexicographical order using bubble sort.
/// @param list The string list.
void StringListSort(char** list);