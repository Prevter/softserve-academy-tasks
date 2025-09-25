#include <cstdio>
#include "AnyType.hpp"

int main() {
    AnyType a = 42;
    AnyType b = 3.14f;
    AnyType c = 'c';
    AnyType d = a;

    // print
    std::printf("a: %d\n", a.get<int>());
    std::printf("b: %f\n", b.get<float>());
    std::printf("c: %c\n", c.get<char>());

    try {
        std::printf("d: %f\n\n", d.get<float>());
    } catch (std::bad_variant_access const& e) {
        std::printf("Caught exception: %s\n\n", e.what());
    }

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
