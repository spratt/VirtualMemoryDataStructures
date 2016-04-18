#ifndef MARRAY_HPP
#define MARRAY_HPP
/******************************************************************************\
 * Multi-dimensional Array
 * MIT License
 * Copyright 2016, Simon Pratt
\******************************************************************************/

#include <iostream>
#include <array>
#include <cmath>
#include <cassert>

#define TABLE_BITS 12

size_t key(uint64_t k, size_t level, size_t N) {
  const uint64_t LEVEL_BITS = 36 / N;
  const uint64_t LEVEL_MASK = pow(2, LEVEL_BITS) - 1;
  const uint64_t shift = ((level-1)*LEVEL_BITS + TABLE_BITS);
  return (k & (LEVEL_MASK << shift)) >> shift;
}

template <std::size_t N, std::size_t M>
struct basic_marray {
  typedef std::array<typename basic_marray<N-1, M>::T*, M> T;

  static uint64_t get(T& m, uint64_t k, size_t topN) {
    size_t level_key = key(k, N, topN);
    // std::cout << std::dec;
    // std::cout << "Getting <" << N << "," << M << ">[" << level_key << "]" << std::endl;
    return basic_marray<N-1, M>::get(*(m[level_key]), k, topN);
  }

  static void set(T& m, uint64_t k, size_t topN, uint64_t v) {
    size_t level_key = key(k, N, topN);
    assert(level_key < M);
    if(m[level_key] == NULL) {
      m[level_key] = new typename basic_marray<N-1, M>::T();
      basic_marray<N-1, M>::init(*(m[level_key]));
      // std::cout << "Alloc: Marray<" << std::dec << N << "," << M << ">[" << level_key << "] = "
      //           << std::hex << m[level_key] << std::endl;
    }
    basic_marray<N-1, M>::set(*(m[level_key]), k, topN, v);
  }

  static void clear(T& m) {
    for(size_t i = 0; i < M; ++i) {
      if(m[i] == NULL) continue;
      basic_marray<N-1, M>::clear(*(m[i]));
      // std::cout << "Free Marray<" << std::dec << N << "," << M << ">[" << i << "] = "
      //           << std::hex << m[i] << std::endl;
      delete m[i];
      m[i] = NULL;
    }
  }

  static void init(T& m) {
    for(size_t i = 0; i < M; ++i) {
      m[i] = (typename basic_marray<N-1, M>::T*)NULL;
    }
  }
};

template <std::size_t M>
struct basic_marray<1, M> {
  typedef std::array<uint64_t, M> T;

  static uint64_t get(T m, uint64_t k, size_t topN) {
    size_t level_key = key(k, 1, topN);
    // std::cout << std::dec;
    // std::cout << "Getting Marray<1," << M << ">[" << level_key << "] = 0x" << std::hex << m[level_key] << std::endl;
    return m[level_key];
  }

  static void set(T& m, uint64_t k, size_t topN, uint64_t v) {
    size_t level_key = key(k, 1, topN);
    // std::cout << std::dec;
    // std::cout << "Setting Marray<1," << M << ">[" << level_key << "] = 0x" << std::hex << v << std::endl;
    m[level_key] = v;
  }

  static void clear(T& m) {
    // Do nothing at this level
  }

  static void init(T& m) {
    for(size_t i = 0; i < M; ++i) {
      m[i] = (uint64_t)NULL;
    }
  }
};

/******************************************************************************/
#ifndef NDEBUG
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
#endif
/******************************************************************************/

template <std::size_t N, std::size_t M>
class Marray {
private:
  using marray = typename basic_marray<N,M>::T;
  using barray = typename basic_marray<1,M>::T;
  
public:

  static const uint8_t KEY_BITS = 36;
  static const uint8_t MIN_LEVELS = 2;
#ifndef NDEBUG
  static_assert( N >= MIN_LEVELS,
                 "Marray: too few levels means stupid big tables" );
  static_assert( KEY_BITS % N == 0,
                 "Marray: #bits should be divisible by #levels (N)");
  static_assert( M == pow_t<2, 36 / N>::value,
                 "Marray: Table size (M) should be 2**(36/N)");
#endif
private:
  marray m;

public:
  Marray() { basic_marray<N,M>::init(m); }
  ~Marray() { clear(); }
  
  uint64_t get(uint64_t k) {
	return basic_marray<N,M>::get(m, k, N);
  }

  void set(uint64_t k, uint64_t v) {
    basic_marray<N,M>::set(m, k, N, v);
  }

  void clear() { basic_marray<N,M>::clear(m); }
};

/******************************************************************************/
#endif
