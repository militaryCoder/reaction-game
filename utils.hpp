#pragma once

#include <cstdlib>

namespace utils
{
    template <typename T>
    T getRandomNumber(T min, T max)
    {
        static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);

        return static_cast<T>(rand() * fraction * (max - min + 1) + min);
    }

    template <typename T>
    bool inRange(T val, T low, T high) { return val >= low && val <= high; }
}
