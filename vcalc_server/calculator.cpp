#include "calculator.h"

int32_t Calculator::product(const int32_t* data, size_t n)
{
    const uint64_t limit = 1ULL << 32; // потолок модуля (больше любого int32_t)
    bool negative = false;             // знак результата
    uint64_t mag = 1;                  // модуль результата (с ограничением)

    for (size_t i = 0; i < n; ++i) {
        if (data[i] == 0)
            return 0;                  // истинное произведение равно нулю
        int64_t v = data[i];
        if (v < 0) {
            negative = !negative;
            v = -v;
        }
        mag *= static_cast<uint64_t>(v);
        if (mag > limit)
            mag = limit;               // дальше модуль только не уменьшается
    }
    if (negative) {
        if (mag > 2147483648ULL)       // меньше -2^31
            return INT32_MIN;
        return static_cast<int32_t>(-static_cast<int64_t>(mag));
    }
    if (mag > static_cast<uint64_t>(INT32_MAX))
        return INT32_MAX;
    return static_cast<int32_t>(mag);
}
