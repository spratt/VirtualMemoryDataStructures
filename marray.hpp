#ifndef MARRAY_HPP
#define MARRAY_HPP
/******************************************************************************\
 * Multi-dimensional Array
 * MIT License
 * Copyright 2016, Simon Pratt
\******************************************************************************/

#include <array>
#include <cmath>
#include <cassert>

#define TABLE_BITS 12

template <std::size_t N, std::size_t M>
struct basic_marray {
  typedef std::array<typename basic_marray<N-1, M>::t*, M> t;
};

template <std::size_t M>
struct basic_marray<1, M> {
  typedef std::array<uint64_t, M> t;
};

template <std::size_t N, std::size_t M>
using marray = typename basic_marray<N,M>::t;

uint16_t key(uint64_t k, size_t level, size_t N) {
  const uint8_t LEVEL_BITS = 36 / N;
  const uint64_t LEVEL_MASK = pow(2, LEVEL_BITS) - 1;
  const uint8_t shift = ((level-1)*LEVEL_BITS + TABLE_BITS);
  return (k & (LEVEL_MASK << shift) >> shift);
}

template <std::size_t L, std::size_t M>
marray<1,M>& get_array(marray<L,M>& m, uint64_t k, size_t N) {
  return get_array<L-1, M>(*(m[key(k, L, N)]), k, N);
}
  
template <std::size_t M>
marray<1,M>& get_array<1,M>(marray<1,M>& m, uint64_t k, size_t N) {
  return m[key(k, 1, N)];
}

/******************************************************************************/
// x^n
// By Alexandre Daigle
template <std::size_t X, std::size_t N>
struct pow_t {
    static constexpr std::size_t value = X * pow_t<X, N-1>::value;
};

template<std::size_t X>
struct pow_t<X, 0> {
    static constexpr std::size_t value = 1;
};
/******************************************************************************/

template <std::size_t N, std::size_t M>
class Marray {
public:
  using barray = marray<1,M>;

  static const uint8_t KEY_BITS = 36;
  static const uint8_t MIN_LEVELS = 2;
  
  static_assert( N >= MIN_LEVELS,
                 "Marray: too few levels means stupid big tables" );
  static_assert( KEY_BITS % N == 0,
                 "Marray: #bits should be divisible by #levels (N)");
  static_assert( M == pow_t<2, 36 / N>::value,
                 "Marray: Table size (M) should be 2**(36/N)");
  
private:
  marray<N, M> m;

  barray& get_table(uint64_t k) {
    return get_array<N, M>(m, k, N);
  }
  
  // barray& get_table(uint64_t k) {
  //   void* next = &m;
  //   for(size_t level = N; level > 1; --level) {
  //     next = ((typename basic_marray<level, M>)*next)[key(k, level)];
  //     assert(next != NULL);
  //   }
  //   return *next;
  // }

public:
  Marray() {}
  
  // uint64_t get(uint64_t k) {
  //   void* p = &m;
  //   for(std:size_t level = 0; level < N; ++level) {
  //     p = (*p)[
  //   }
  // }

  void set(uint64_t k, uint64_t v) {
    get_table(k)[key(k, 1, N)] = v;
    return;
  }
};

/******************************************************************************/
#endif
