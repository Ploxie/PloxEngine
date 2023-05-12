//
// Created by Ploxie on 2023-05-11.
//

#pragma once
#include <cstdint>

#define DEF_ENUM_FLAG_OPERATORS(ENUMTYPE)                                                                                       \
    inline constexpr ENUMTYPE operator|(ENUMTYPE a, ENUMTYPE b) noexcept                                                        \
    {                                                                                                                           \
	return ENUMTYPE(((ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type) a) | ((ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type) b));         \
    }                                                                                                                           \
    inline ENUMTYPE &operator|=(ENUMTYPE &a, ENUMTYPE b) noexcept                                                               \
    {                                                                                                                           \
	return (ENUMTYPE &) (((ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &) a) |= ((ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type) b)); \
    }                                                                                                                           \
    inline constexpr ENUMTYPE operator&(ENUMTYPE a, ENUMTYPE b) noexcept                                                        \
    {                                                                                                                           \
	return ENUMTYPE(((ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type) a) & ((ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type) b));         \
    }                                                                                                                           \
    inline ENUMTYPE &operator&=(ENUMTYPE &a, ENUMTYPE b) noexcept                                                               \
    {                                                                                                                           \
	return (ENUMTYPE &) (((ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &) a) &= ((ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type) b)); \
    }                                                                                                                           \
    inline constexpr ENUMTYPE operator~(ENUMTYPE a) noexcept                                                                    \
    {                                                                                                                           \
	return ENUMTYPE(~((ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type) a));                                                        \
    }                                                                                                                           \
    inline constexpr ENUMTYPE operator^(ENUMTYPE a, ENUMTYPE b) noexcept                                                        \
    {                                                                                                                           \
	return ENUMTYPE(((ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type) a) ^ ((ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type) b));         \
    }                                                                                                                           \
    inline ENUMTYPE &operator^=(ENUMTYPE &a, ENUMTYPE b) noexcept                                                               \
    {                                                                                                                           \
	return (ENUMTYPE &) (((ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &) a) ^= ((ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type) b)); \
    }                                                                                                                           \
    inline constexpr bool operator==(ENUMTYPE a, ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type b) noexcept                            \
    {                                                                                                                           \
	return (ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type) a == b;                                                                \
    }                                                                                                                           \
    inline constexpr bool operator!=(ENUMTYPE a, ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type b) noexcept                            \
    {                                                                                                                           \
	return (ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type) a != b;                                                                \
    }

template<size_t S>
struct ENUM_FLAG_INTEGER_FOR_SIZE;

template<>
struct ENUM_FLAG_INTEGER_FOR_SIZE<1>
{
    using type = int8_t;
};

template<>
struct ENUM_FLAG_INTEGER_FOR_SIZE<2>
{
    using type = int16_t;
};

template<>
struct ENUM_FLAG_INTEGER_FOR_SIZE<4>
{
    using type = int32_t;
};

template<>
struct ENUM_FLAG_INTEGER_FOR_SIZE<8>
{
    using type = int64_t;
};

template<class T>
struct ENUM_FLAG_SIZED_INTEGER
{
    using type = typename ENUM_FLAG_INTEGER_FOR_SIZE<sizeof(T)>::type;
};