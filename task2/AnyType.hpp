#pragma once
#include <optional>
#include <type_traits>
#include <variant>

class AnyType {
public:
    enum class Type {
        Bool,
        Char,
        UChar,
        Short,
        UShort,
        Int,
        UInt,
        Long,
        ULong,
        LongLong,
        ULongLong,
        Float,
        Double,
        LongDouble,
    };

    template <typename T> requires std::is_arithmetic_v<T>
    consteval static Type type_of() noexcept {
        if constexpr (std::is_same_v<T, bool>) return Type::Bool;
        if constexpr (std::is_same_v<T, char>) return Type::Char;
        if constexpr (std::is_same_v<T, unsigned char>) return Type::UChar;
        if constexpr (std::is_same_v<T, short>) return Type::Short;
        if constexpr (std::is_same_v<T, unsigned short>) return Type::UShort;
        if constexpr (std::is_same_v<T, int>) return Type::Int;
        if constexpr (std::is_same_v<T, unsigned int>) return Type::UInt;
        if constexpr (std::is_same_v<T, long>) return Type::Long;
        if constexpr (std::is_same_v<T, unsigned long>) return Type::ULong;
        if constexpr (std::is_same_v<T, long long>) return Type::LongLong;
        if constexpr (std::is_same_v<T, unsigned long long>) return Type::ULongLong;
        if constexpr (std::is_same_v<T, float>) return Type::Float;
        if constexpr (std::is_same_v<T, double>) return Type::Double;
        if constexpr (std::is_same_v<T, long double>) return Type::LongDouble;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    T& as() noexcept {
        if constexpr (std::is_same_v<T, bool>) return m_data.b;
        if constexpr (std::is_same_v<T, char>) return m_data.c;
        if constexpr (std::is_same_v<T, unsigned char>) return m_data.uc;
        if constexpr (std::is_same_v<T, short>) return m_data.s;
        if constexpr (std::is_same_v<T, unsigned short>) return m_data.us;
        if constexpr (std::is_same_v<T, int>) return m_data.i;
        if constexpr (std::is_same_v<T, unsigned int>) return m_data.ui;
        if constexpr (std::is_same_v<T, long>) return m_data.l;
        if constexpr (std::is_same_v<T, unsigned long>) return m_data.ul;
        if constexpr (std::is_same_v<T, long long>) return m_data.ll;
        if constexpr (std::is_same_v<T, unsigned long long>) return m_data.ull;
        if constexpr (std::is_same_v<T, float>) return m_data.f;
        if constexpr (std::is_same_v<T, double>) return m_data.d;
        if constexpr (std::is_same_v<T, long double>) return m_data.ld;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    explicit(false) AnyType(T value) noexcept : m_type(type_of<T>()) { as<T>() = value; }

    AnyType(AnyType const& other) noexcept = default;
    AnyType(AnyType&& other) noexcept = default;
    AnyType& operator=(AnyType const& other) noexcept = default;
    AnyType& operator=(AnyType&& other) noexcept = default;

    [[nodiscard]] Type type() const noexcept { return m_type; }

    template <typename T> requires std::is_arithmetic_v<T>
    [[nodiscard]] bool is() const noexcept {
        return m_type == type_of<T>();
    }

    void swap(AnyType& other) noexcept {
        std::swap(m_data, other.m_data);
        std::swap(m_type, other.m_type);
    }

    template <typename T> requires std::is_arithmetic_v<T>
    [[nodiscard]] T get() const {
        if (!is<T>()) throw std::bad_variant_access();
        return const_cast<AnyType*>(this)->as<T>();
    }

    template <typename T> requires std::is_arithmetic_v<T>
    [[nodiscard]] std::optional<T> try_get() const noexcept {
        if (!is<T>()) return std::nullopt;
        return const_cast<AnyType*>(this)->as<T>();
    }

private:
    union {
        bool b;
        char c;
        unsigned char uc;
        short s;
        unsigned short us;
        int i;
        unsigned int ui;
        long l;
        unsigned long ul;
        long long ll;
        unsigned long long ull;
        float f;
        double d;
        long double ld;
    } m_data{};
    Type m_type{};
};