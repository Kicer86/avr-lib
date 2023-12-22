
#ifndef MATH_HPP_INCLUDED
#define MATH_HPP_INCLUDED


template<typename T>
constexpr T abs(T a)
{
    return a < 0? -a : a;
}


template<typename T, typename F>
constexpr T round(F v)
{
    return static_cast<T>(v + static_cast<F>(v < 0? - 0.5 : 0.5));
}


// verify correctness
static_assert(abs(1) == 1);
static_assert(abs(-1) == 1);
static_assert(abs(-5) == 5);
static_assert(abs(2) == 2);

static_assert(round<int>(0.1) == 0);
static_assert(round<int>(0.49) == 0);
static_assert(round<int>(0.5) == 1);
static_assert(round<int>(0.9) == 1);
static_assert(round<int>(-0.1) == 0);
static_assert(round<int>(-0.49) == 0);
static_assert(round<int>(-0.5) == -1);
static_assert(round<int>(-0.9) == -1);


#endif
