#ifndef VOIDJS_UTILS_HELPER_H
#define VOIDJS_UTILS_HELPER_H

#include <cstdint>
#include <limits>
#include <string>
#include <codecvt>
#include <locale>
#include <cmath>
#include <type_traits>
#include <vector>
#include <cstring>

#include "voidjs/lexer/character.h"

namespace voidjs {
namespace utils {

// This code comes from https://github.com/zhuzilin/es/blob/main/es/utils/helper.h
inline std::u16string U8StrToU16Str(std::string u8str) {
  static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  return convert.from_bytes(u8str);
}

// This code comes from https://github.com/zhuzilin/es/blob/main/es.cc
inline std::string U16StrToU8Str(std::u16string u16str) {
  static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  return convert.to_bytes(u16str);
}


template <typename T, typename U>
union Data {
  T t;
  U u;
};

// replace for std::bit_cast, which is introduced in C++20
template <typename To, typename From>
inline To BitCast(const From& src) {
  static_assert(sizeof(To) == sizeof(From));
  Data<From, To> data;
  data.t = src;
  return data.u;
}

template <typename T>
inline T BitGet(const void* start, std::size_t offset) {
  return reinterpret_cast<T>(reinterpret_cast<std::uintptr_t>(start) + offset);
}

template <typename T, std::uint64_t START_BIT, std::uint64_t END_BIT>
struct BitSet {
  // T must be integer or enum
  static_assert(std::is_integral_v<T> || std::is_enum_v<T>);
  
  // 0 <= START_BIT <= END_BIT < 64
  static_assert(0 <= START_BIT);
  static_assert(START_BIT <= END_BIT);
  static_assert(END_BIT <= 64);

  static constexpr std::uint64_t Mask() {
    return ((1ull << (END_BIT - START_BIT)) - 1) << START_BIT; 
  }
  
  static constexpr std::uint64_t Encode(T value) {
    return (static_cast<std::uint64_t>(value) << START_BIT);
  }

  static constexpr T Decode(std::uint64_t base) {
    return static_cast<T>((base & Mask()) >> START_BIT);
  }

  static constexpr void Set(std::uint64_t* ptr, T value) {
    *ptr = (*ptr & ~Mask()) | Encode(value);
  }

  static constexpr T Get(std::uint64_t base) {
    return Decode(base);
  }
};

static constexpr std::uint64_t DOUBLE_SIGNIFICAND_BITS = 52;
static constexpr std::uint64_t DOUBLE_SIGNIFICAND_MASK = 0x000'FFFFFFFFFFFFF;

static constexpr std::uint64_t DOUBLE_HIDDEN_BIT       = 1ull << DOUBLE_SIGNIFICAND_BITS;

static constexpr std::uint64_t DOUBLE_EXPONENT_BITS    = 11;
static constexpr std::uint64_t DOUBLE_EXPONENT_MASK    = 0x7FF'0000000000000;
static constexpr std::uint64_t DOUBLE_EXPONENT_BIAS    = (1ull << (DOUBLE_EXPONENT_BITS - 1)) - 1;

static constexpr std::uint64_t DOUBLE_SIGN_BITS        = 1; 
static constexpr std::uint64_t DOUBLE_SIGN_MASK        = 0x800'0000000000000;

// This code comes from 
template <std::size_t BITS>
inline std::int32_t DoubleToInt(double d) {
  static_assert(BITS == 8 || BITS == 16 || BITS == 32 || BITS == 64,
                "invalid bits for DoubleToInt convert");
  int32_t ret = 0;
  auto u64 = BitCast<uint64_t>(d);
  int exp = static_cast<int>((u64 & DOUBLE_EXPONENT_MASK) >> DOUBLE_SIGNIFICAND_BITS) - DOUBLE_EXPONENT_BIAS;
  if (exp < static_cast<int>(BITS - 1)) {
    // smaller than INT<BITS>_MAX, fast conversion
    ret = static_cast<int32_t>(d);
  } else if (exp < static_cast<int>(BITS + DOUBLE_SIGNIFICAND_BITS)) {
    // Still has significand bits after mod 2^<BITS>
    // Get low <BITS> bits by shift left <64 - BITS> and shift right <64 - BITS>
    uint64_t value = (((u64 & DOUBLE_SIGNIFICAND_MASK) | DOUBLE_HIDDEN_BIT)
                      << (static_cast<uint32_t>(exp) - DOUBLE_SIGNIFICAND_BITS + sizeof(std::uint64_t) - BITS)) >>
      (sizeof(std::uint64_t) - BITS);
    ret = static_cast<int32_t>(value);
    if ((u64 & DOUBLE_SIGN_MASK) == DOUBLE_SIGN_MASK && ret != INT32_MIN) {
      ret = -ret;
    }
  } else {
    // No significand bits after mod 2^<bits>, contains NaN and INF
    ret = 0;
  }
  return ret;
}

// CanDoubleConvertToInt32
// If number is an integer in the range of std::int32_t, return true;
// otherwise, return fales.
inline bool CanDoubleConvertToInt32(double number) {
  if (std::fabs(number) <= static_cast<double>(std::numeric_limits<std::int32_t>::max())) {
    auto num = static_cast<std::int32_t>(number);
    return static_cast<double>(num) == number;
  } else {
    return false;
  }
}

inline double TruncateDouble(double d) {
    if (std::isnan(d)) {
        return 0;
    }
    if (std::isinf(d)) {
        return d;
    }
    // -0 to +0
    if (d == 0.0) {
        return 0;
    }
    double ret = d >= 0 ? std::floor(d) : std::ceil(d);
    // -0 to +0
    if (ret == 0.0) {
        ret = 0;
    }
    return ret;
}

// The Code in namespace detail is copied directly from ankerl::unordered_dense: https://github.com/martinus/unordered_dense
namespace detail {
namespace wyhash {
// This is a stripped-down implementation of wyhash: https://github.com/wangyi-fudan/wyhash
// No big-endian support (because different values on different machines don't matter),
// hardcodes seed and the secret, reformats the code, and clang-tidy fixes.
inline void mum(uint64_t* a, uint64_t* b) {
#    if defined(__SIZEOF_INT128__)
    __uint128_t r = *a;
    r *= *b;
    *a = static_cast<uint64_t>(r);
    *b = static_cast<uint64_t>(r >> 64U);
#    elif defined(_MSC_VER) && defined(_M_X64)
    *a = _umul128(*a, *b, b);
#    else
    uint64_t ha = *a >> 32U;
    uint64_t hb = *b >> 32U;
    uint64_t la = static_cast<uint32_t>(*a);
    uint64_t lb = static_cast<uint32_t>(*b);
    uint64_t hi{};
    uint64_t lo{};
    uint64_t rh = ha * hb;
    uint64_t rm0 = ha * lb;
    uint64_t rm1 = hb * la;
    uint64_t rl = la * lb;
    uint64_t t = rl + (rm0 << 32U);
    auto c = static_cast<uint64_t>(t < rl);
    lo = t + (rm1 << 32U);
    c += static_cast<uint64_t>(lo < t);
    hi = rh + (rm0 >> 32U) + (rm1 >> 32U) + c;
    *a = lo;
    *b = hi;
#    endif
}

// multiply and xor mix function, aka MUM
[[nodiscard]] inline auto mix(uint64_t a, uint64_t b) -> uint64_t {
    mum(&a, &b);
    return a ^ b;
}

// read functions. WARNING: we don't care about endianness, so results are different on big endian!
[[nodiscard]] inline auto r8(const uint8_t* p) -> uint64_t {
    uint64_t v{};
    std::memcpy(&v, p, 8U);
    return v;
}

[[nodiscard]] inline auto r4(const uint8_t* p) -> uint64_t {
    uint32_t v{};
    std::memcpy(&v, p, 4);
    return v;
}

// reads 1, 2, or 3 bytes
[[nodiscard]] inline auto r3(const uint8_t* p, size_t k) -> uint64_t {
    return (static_cast<uint64_t>(p[0]) << 16U) | (static_cast<uint64_t>(p[k >> 1U]) << 8U) | p[k - 1];
}

[[maybe_unused]] [[nodiscard]] inline auto hash(void const* key, size_t len) -> uint64_t {
    static constexpr auto secret = std::array{UINT64_C(0xa0761d6478bd642f),
                                              UINT64_C(0xe7037ed1a0b428db),
                                              UINT64_C(0x8ebc6af09c88c6e3),
                                              UINT64_C(0x589965cc75374cc3)};

    auto const* p = static_cast<uint8_t const*>(key);
    uint64_t seed = secret[0];
    uint64_t a{};
    uint64_t b{};
    if ((len <= 16)) {
        if ((len >= 4)) {
            a = (r4(p) << 32U) | r4(p + ((len >> 3U) << 2U));
            b = (r4(p + len - 4) << 32U) | r4(p + len - 4 - ((len >> 3U) << 2U));
        } else if ((len > 0)) {
            a = r3(p, len);
            b = 0;
        } else {
            a = 0;
            b = 0;
        }
    } else {
        size_t i = len;
        if ((i > 48)) {
            uint64_t see1 = seed;
            uint64_t see2 = seed;
            do {
                seed = mix(r8(p) ^ secret[1], r8(p + 8) ^ seed);
                see1 = mix(r8(p + 16) ^ secret[2], r8(p + 24) ^ see1);
                see2 = mix(r8(p + 32) ^ secret[3], r8(p + 40) ^ see2);
                p += 48;
                i -= 48;
            } while ((i > 48));
            seed ^= see1 ^ see2;
        }
        while ((i > 16)) {
            seed = mix(r8(p) ^ secret[1], r8(p + 8) ^ seed);
            i -= 16;
            p += 16;
        }
        a = r8(p + i - 16);
        b = r8(p + i - 8);
    }

    return mix(secret[1] ^ len, mix(a ^ secret[1], b ^ seed));
}

}  // namespace wyhash

template <typename T, typename Enable = void>
struct hash {
    auto operator()(T const& obj) const noexcept(noexcept(std::declval<std::hash<T>>().operator()(std::declval<T const&>())))
        -> uint64_t {
        return std::hash<T>{}(obj);
    }
};

template <typename CharT>
struct hash<std::basic_string<CharT>> {
    using is_avalanching = void;
    auto operator()(std::basic_string<CharT> const& str) const noexcept -> uint64_t {
        return wyhash::hash(str.data(), sizeof(CharT) * str.size());
    }
};

template <typename CharT>
struct hash<std::basic_string_view<CharT>> {
    using is_avalanching = void;
    auto operator()(std::basic_string_view<CharT> const& sv) const noexcept -> uint64_t {
        return wyhash::hash(sv.data(), sizeof(CharT) * sv.size());
    }
};

}  // namespace detail

}  // namespace utils
}  // namespace voidjs

#endif  // VOIDJS_UTILS_HELPER_H
