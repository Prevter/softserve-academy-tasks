## SoftServe Academy Tasks

### Task 1: Implement a dynamically sized list of c-strings.

**Requirements:**

1) Only stdlib.h and string.h headers can be used in file where the list is implemented.
2) No new or delete operators. Only malloc, realloc and free functions.
3) Do not use classes or structures. Only pointers can be used.
4) You can use ANY code convention you like
5) User should be able to:
    - add/remove string
    - get the number of items in the list
    - get the index position of the first exact match of specified string

**Will be a plus:**

User is able to remove duplicates, replace the text in items, sort list.

### Task 2: Structures

**As a** developer  
**I want** to have a class that can store single values of C++ fundamental type
(except void and std::nullptr_t) and a function/method performs type-safe access to the contained value.
**so that** I will be able to use a single variable to store an int, then a double, then a bool etc.

**Acceptance Criteria:**

- The class should contain all type of constructors
- The class should contain all type of assignment operators
- The class should contain a method to destroy contained object
- The class should contain a method to swap two objects
- The class should contain a method returns the type of the contained value
- The function/method that extracts a value should provide type-safe access. If the requested type does not match to
  stored type exception should be thrown

### Task 3: Threads, containers, input/output with files Skip to end of metadata

**As a** developer **I want** to have an application that counts blank lines, comment lines, and physical lines of source code
written in C/C++. **so that** I can have some internal statistic of the project.

**Acceptance Criteria**
1. There are should be a way to specify project root folder
2. Only files with *.h, *.hpp, *.c, *.cpp extensions are processed from all subfolders
3. The output should contain the total number of processed files, count of blank lines, count of comment lines, count of lines with code and time of execution
4. The results should also be saved in file. 
