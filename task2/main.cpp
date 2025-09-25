#include <cstdio>
#include "AnyType.hpp"

int main() {
    AnyType a = 42;
    AnyType b = 3.14f;
    AnyType c = 'c';
    AnyType d = a;

    a = 100;
    d = b;
    b = 2.71f;
    a.swap(c);

    // print
    std::printf("a: %c\n", a.get<char>());
    std::printf("b: %f\n", b.get<float>());
    std::printf("c: %d\n", c.get<int>());
    std::printf("d: %f\n", d.get<float>());

    return 0;
}
