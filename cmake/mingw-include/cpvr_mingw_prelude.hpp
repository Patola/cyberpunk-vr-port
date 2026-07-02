#pragma once

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <functional>
#include <utility>

#if defined(__MINGW32__) && !defined(_MSC_VER)
#ifndef __try
#define __try try
#endif
#ifndef __except
#define __except(expr) catch (...)
#endif

inline char InterlockedExchange8(volatile char* destination, char value)
{
    return __atomic_exchange_n(destination, value, __ATOMIC_SEQ_CST);
}

inline char _InterlockedExchangeAdd8(volatile char* addend, char value)
{
    return __atomic_fetch_add(addend, value, __ATOMIC_SEQ_CST);
}

template <size_t Size, typename... Args>
inline int sprintf_s(char (&buffer)[Size], const char* format, Args... args)
{
    const int written = std::snprintf(buffer, Size, format, args...);
    if (written < 0 || static_cast<size_t>(written) >= Size) {
        buffer[Size - 1] = '\0';
    }
    return written;
}
#endif
