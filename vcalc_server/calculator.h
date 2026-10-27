#pragma once
#include <cstddef>
#include <cstdint>

// Вычислитель
class Calculator {
public:
    // произведение значений вектора int32_t
    // при переполнении вверх возвращает 2^31-1, вниз -2^31
    // произведение пустого вектора равно 1
    static int32_t product(const int32_t* data, size_t n);
};
