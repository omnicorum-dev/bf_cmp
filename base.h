//
// Created by Nico Russo on 4/12/26.
//

#ifndef BASE_H
#define BASE_H

#include <complex>
#include <iostream>
#include <string>
#include <stdexcept>

// ========================================================
// BASIC TYPES
// ========================================================
namespace omni::basic {
    using i8  = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;

    using u8  = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    using f32 = float;
    using f64 = double;
}

// ========================================================
// BASIC CONSTANTS
// ========================================================
namespace omni::basic {
    using namespace std::complex_literals;
    constexpr std::complex<float> I = 1if;

    constexpr i8  min_i8  = static_cast<i8>(0x80);
    constexpr i16 min_i16 = static_cast<i16>(0x8000);
    constexpr i32 min_i32 = static_cast<i32>(0x80000000);
    constexpr i64 min_i64 = static_cast<i64>(0x8000000000000000);

    constexpr i8  max_i8  = 0x7F;
    constexpr i16 max_i16 = 0x7FFF;
    constexpr i32 max_i32 = 0x7FFFFFFF;
    constexpr i64 max_i64 = 0x7FFFFFFFFFFFFFFF;

    constexpr u8  max_u8  = 0xFF;
    constexpr u16 max_u16 = 0xFFFF;
    constexpr u32 max_u32 = 0xFFFFFFFF;
    constexpr u64 max_u64 = 0xFFFFFFFFFFFFFFFF;

    constexpr f32 machine_epsilon_f32  = 1.1920929e-7f;
    constexpr f32 pi_f32               = 3.14159265359f;
    constexpr f32 tau_f32              = 6.28318530718f;
    constexpr f32 e_f32                = 2.71828182846f;
    constexpr f32 gold_big_f32         = 1.61803398875f;
    constexpr f32 gold_small_f32       = 0.61803398875f;

    constexpr f64 machine_epsilon_f64  = 2.220446e-16;
    constexpr f64 pi_f64               = 3.14159265359;
    constexpr f64 tau_f64              = 6.28318530718;
    constexpr f64 two_pi_f64           = 6.28318530718;
    constexpr f64 e_f64                = 2.71828182846;
    constexpr f64 gold_big_f64         = 1.61803398875;
    constexpr f64 gold_small_f64       = 0.61803398875;
}

// ========================================================
// USEFUL FUNCTIONS
// ========================================================
namespace omni::basic {
    static f32 lerp   (const f32 a, const f32 b, const f32 t) { return a + (b - a) * t; }
    static f32 unlerp (const f32 a, const f32 b, const f32 x) { if (a!=b) { return (x-a)/(b-a); } return 0.f; }

    inline f32 rand_f32() { return static_cast<f32>(std::rand()) / static_cast<f32>(RAND_MAX); }

    inline i32 rand_i32(const i32 min, const i32 max) { return min + std::rand() % ((max - min) + 1); }

    inline f32 intToFloat(const i32 x) { return static_cast<f32>(x) / static_cast<f32>(max_i32); }

    static f32 catmullRomSpline(const f32 p0, const f32 p1, const f32 p2, const f32 p3, const f32 t) {
        const f32 t2 = t * t;
        const f32 t3 = t2 * t;
        return 0.5f * (
            2.0f * p1 +
            (-p0 + p2) * t +
            (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
            (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
        );
    }
}

// ========================================================
// PRINTING AND LOGGING
// ========================================================
namespace omni::basic {

#define ANSI_BLACK      "\033[30m"
#define ANSI_RED        "\033[31m"
#define ANSI_GREEN      "\033[32m"
#define ANSI_YELLOW     "\033[33m"
#define ANSI_BLUE       "\033[34m"
#define ANSI_MAGENTA    "\033[35m"
#define ANSI_CYAN       "\033[36m"
#define ANSI_WHITE      "\033[37m"
#define ANSI_GREY       "\033[90m"

#define ANSI_BOLD       "\033[1m"
#define ANSI_ITALIC     "\033[3m"
#define ANSI_UNDERLINE  "\033[4m"

#define ANSI_RESET      "\033[0m"

    inline void fprint_impl(std::ostream& os, const std::string& fmt, size_t pos) {
        // if there are still {} and no more arguments -> error
        if (fmt.find("{}", pos) != std::string::npos) {
            throw std::runtime_error("Too few arguments for format string");
        }

        os << fmt.substr(pos);
    }

    template<typename T, typename... Args>
    void fprint_impl(std::ostream& os, const std::string& fmt, size_t pos, T value, Args... args) {
        const size_t brace = fmt.find("{}", pos);

        // if there are no more {} but there are still args -> error
        if (brace == std::string::npos) {
            throw std::runtime_error("Too many arguments for format string");
        }

        os << fmt.substr(pos, brace-pos);
        os << value;
        fprint_impl(os, fmt, brace + 2, args...);
    }

    template<typename... Args>
    void print(const std::string& fmt, Args... args) {
        fprint_impl(std::cout, fmt, 0, args...);
    }

    template<typename... Args>
    void print(std::ostream& os, const std::string& fmt, Args... args) {
        fprint_impl(os, fmt, 0, args...);
    }

    template<typename... Args>
    void println(const std::string& fmt, Args... args) {
        fprint_impl(std::cout, fmt, 0, args...);
        std::cout << std::endl;
    }

    template<typename... Args>
    void println(std::ostream& os, const std::string& fmt, Args... args) {
        fprint_impl(std::cout, fmt, 0, args...);
        std::cout << std::endl;
    }

    template<typename... Args>
    void LOG_FATAL(const std::string& fmt, Args... args) {
        std::cerr << ANSI_RED << ANSI_BOLD << "[FATAL] ";
        print(std::cerr, fmt, args...);
        std::cerr << ANSI_RESET << std::endl;
    }

    template<typename... Args>
    void LOG_ERROR(const std::string& fmt, Args... args) {
        std::cerr << ANSI_RED << "[ERROR] " << ANSI_ITALIC;
        print(std::cerr, fmt, args...);
        std::cerr << ANSI_RESET << std::endl;
    }

    template<typename... Args>
    void LOG_WARN(const std::string& fmt, Args... args) {
        #ifndef DISABLE_WARN
        std::cerr << ANSI_MAGENTA << "[WARN] ";
        print(std::cerr, fmt, args...);
        std::cerr << ANSI_RESET << std::endl;
        #endif
    }

    template<typename... Args>
    void LOG_INFO(const std::string& fmt, Args... args) {
        #ifndef DISABLE_INFO
        std::cerr << ANSI_GREEN << "[INFO] ";
        print(std::cerr, fmt, args...);
        std::cerr << ANSI_RESET << std::endl;
        #endif
    }

    template<typename... Args>
    void LOG_DEBUG(const std::string& fmt, Args... args) {
        #ifndef DISABLE_DEBUG
        std::cerr << ANSI_CYAN << "[DEBUG] " << ANSI_ITALIC;
        print(std::cerr, fmt, args...);
        std::cerr << ANSI_RESET << std::endl;
        #endif
    }

    template<typename... Args>
    void LOG_TRACE(const std::string& fmt, Args... args) {
        #ifndef DISABLE_TRACE
        std::cerr << ANSI_GREY << "[TRACE] " << ANSI_ITALIC;
        print(std::cerr, fmt, args...);
        std::cerr << ANSI_RESET << std::endl;
        #endif
    }

    template<typename... Args>
    void panic(const std::string& fmt, Args... args) {
        LOG_FATAL(fmt, args...);
        std::terminate();
    }

    template<typename... Args>
    void TODO(const std::string& fmt, Args... args) {
        LOG_DEBUG(fmt, args...);
    }
}

// ========================================================
// USEFUL MACROS
// ========================================================
namespace omni::basic {
#define Assert(c) \
    do { \
        if (!(c)) { \
            panic("Assertion failed: " #c); \
        } \
    } while (0)

#define WarnIfNot(c, msg, ...) \
    do { \
        if (!(c)) { \
            omni::LOG_WARN(msg, ##__VA_ARGS__); \
        } \
    } while (0)

#define ArrayCount(a)       (sizeof(a) / sizeof(a[0]))

#define IntFromPtr(p)       (unsigned long long)((char*)p - (char*)0)
#define PtrFromInt(n)       (void*)((char*)0 + (n))

#define Member(T,m)         (((T*)0)->m)
#define OffsetOfMember(T,m) IntFromPtr(&Member(T,m))

}

// ========================================================
// COMPOUND TYPES
// ========================================================
namespace omni {
    using namespace basic;

    template<typename T>
    struct Vec2 {
        union {
            struct { T x, y; };
            struct { T L, R; };
            struct { T u, v; };
            T data[2];
        };

        Vec2() : x(T()), y(T()) {}
        Vec2(const T x, const T y) : x(x), y(y) {}

        T& operator[] (const size_t i) {
            if (i > 1) exit(1);
            return data[i];
        }

        const T& operator[] (const size_t i) const {
            if (i > 1) exit(1);
            return data[i];
        }

        friend std::ostream& operator<<(std::ostream& os, const Vec2<T>& v) {
            return os << v.x << ", " << v.y;
        }

        friend Vec2<T> operator+(const Vec2<T>& a, const Vec2<T>& b) {
            return Vec2<T>(a.x + b.x, a.y + b.y);
        }

        friend Vec2<T> operator-(const Vec2<T>& a, const Vec2<T>& b) {
            return Vec2<T>(a.x - b.x, a.y - b.y);
        }

        friend Vec2<T> operator*(const Vec2<T>& a, const Vec2<T>& b) {
            return Vec2<T>(a.x * b.x, a.y * b.y);
        }

        friend Vec2<T> operator/(const Vec2<T>& a, const Vec2<T>& b) {
            return Vec2<T>(a.x / b.x, a.y / b.y);
        }

        friend constexpr T dot(const Vec2<T>& a, const Vec2<T>& b) {
            return a.x * b.x + a.y * b.y;
        }

        friend inline T length(const Vec2<T>& a) {
            return sqrt(dot(a, a));
        }

        friend inline Vec2<T> normalize(const Vec2<T>& a) {
            T len = length(a);
            if (len == T(0)) return a;
            return Vec2<T>(a.x / len, a.y / len);
        }
    };

    template<typename T>
    struct Vec3 {
        union {
            struct { T x, y, z; };
            struct { T r, g, b; };
            struct { T L, R, C; };
            struct { T u, v, w; };
            T data[3];
        };

        Vec3() : x(T()), y(T()), z(T()) {}
        Vec3(const T x, const T y, const T z) : x(x), y(y), z(z) {}

        T& operator[] (const size_t i) {
            if (i > 2) exit(1);
            return data[i];
        }

        const T& operator[] (const size_t i) const {
            if (i > 2) exit(1);
            return data[i];
        }

        friend std::ostream& operator<<(std::ostream& os, const Vec3<T>& v) {
            return os << v.x << ", " << v.y << ", " << v.z;
        }

        friend Vec3<T> operator+(const Vec3<T>& a, const Vec3<T>& b) {
            return Vec3<T>(a.x + b.x, a.y + b.y, a.z + b.z);
        }

        friend Vec3<T> operator-(const Vec3<T>& a, const Vec3<T>& b) {
            return Vec3<T>(a.x - b.x, a.y - b.y, a.z - b.z);
        }

        friend Vec3<T> operator*(const Vec3<T>& a, const Vec3<T>& b) {
            return Vec3<T>(a.x * b.x, a.y * b.y, a.z * b.z);
        }

        friend Vec3<T> operator/(const Vec3<T>& a, const Vec3<T>& b) {
            return Vec3<T>(a.x / b.x, a.y / b.y, a.z / b.z);
        }

        friend inline constexpr T dot(const Vec3<T>& a, const Vec3<T>& b) {
            return a.x * b.x + a.y * b.y + a.z * b.z;
        }

        friend inline T length(const Vec3<T>& a) {
            return sqrt(dot(a, a));
        }

        friend inline Vec3<T> normalize(const Vec3<T>& a) {
            T len = length(a);
            if (len == T(0)) return a;
            return Vec3<T>(a.x / len, a.y / len, a.z / len);
        }
    };

    template<typename T>
    struct Vec4 {
        union {
            struct { T x, y, z, w; };
            struct { T r, g, b, a; };
            T data[4];
        };

        Vec4() : x(T()), y(T()), z(T()), w(T()) {}
        Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

        T& operator[] (const size_t i) {
            if (i > 3) exit(1);
            return data[i];
        }

        const T& operator[] (const size_t i) const {
            if (i > 3) exit(1);
            return data[i];
        }

        friend std::ostream& operator<<(std::ostream& os, const Vec4<T>& v) {
            return os << v.x << ", " << v.y << ", " << v.z << ", " << v.w;
        }

        friend Vec4<T> operator+(const Vec4<T>& a, const Vec4<T>& b) {
            return Vec4<T>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
        }

        friend Vec4<T> operator-(const Vec4<T>& a, const Vec4<T>& b) {
            return Vec4<T>(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
        }

        friend Vec4<T> operator*(const Vec4<T>& a, const Vec4<T>& b) {
            return Vec4<T>(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
        }

        friend Vec4<T> operator/(const Vec4<T>& a, const Vec4<T>& b) {
            return Vec4<T>(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
        }

        friend inline constexpr T dot(const Vec4<T>& a, const Vec4<T>& b) {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        }

        friend inline T length(const Vec4<T>& a) {
            return sqrt(dot(a, a));
        }

        friend inline Vec4<T> normalize(const Vec4<T>& a) {
            T len = length(a);
            if (len == T(0)) return a;
            return Vec4<T>(a.x / len, a.y / len, a.z / len, a.w);
        }
    };

    template<typename T, typename E>
    struct Pair {
        union {
            struct { T first; E second; };
            struct { T a; E b; };
            struct { T x; E y; };
            struct { T key; E value; };
        };

        Pair() : a(T()), b(E()) {}
        Pair(T a, E b) : a(a), b(b) {}

        friend std::ostream& operator<<(std::ostream& os, const Pair<T, E>& p) {
            return os << "[" << p.a << ", " << p.b << "]";
        }

        bool operator==(const Pair& p) const {
            return a == p.a;
        }
    };
}

#endif //BASE_H
