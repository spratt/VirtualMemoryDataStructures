#include <iostream>
#include <vector>
#include <bitset>
#include <random>
#include <cstdint>
#include "radixtree.hpp"

// Given a 64-bit number vadd, returns the same number but with the
// most-significant 16-bits (63:48) replaced with the value of bit 47,
// effectively sign-extending it to make it a valid virtual address in
// x86_64.
//
// 6  6         5  4      4         3         2         1         0
// 3210987654321098765432109876543210987654321098765432109876543210
// ----------------------------------------------------------------
// \______________/\_______/\_______/\_______/\_______/\__________/
//  sign extend(16)  p4(9)    p3(9)    p2(9)    p1(9)   offset(12)
//
uint64_t correctify_vadd(uint64_t vadd) {
  // 48 0'x
  uint64_t bit47 = 0b1;
  bit47 <<= 47;
  if(vadd & bit47) {
    uint64_t mask = 0b1111111111111111;
    mask <<= 48;
    return vadd | mask;
  } else {
    uint64_t mask = 0b111111111111111111111111111111111111111111111111;
    return vadd & mask;
  }
}

// Given 64-bit numbers vadd and padd, return a valid physical address
// with the following form:
//
// 6  6         5  4      4         3         2         1         0
// 3210987654321098765432109876543210987654321098765432109876543210
// ----------------------------------------------------------------
// 0000000000000000000000001000000010000000011000000100000000000000  <- example
// \__________/\______________________________________/\__________/
//  ignore(12)                padd(40)                   vadd(12)
//
// That is, return the least-significant 12 bits (11:0) from vadd
// bitwise-ORed with bits 47:12 from padd.
//
uint64_t correctify_padd(uint64_t vadd, uint64_t padd) {
  uint64_t vmask = 0b111111111111;
  uint64_t pmask = 0b1111111111111111111111111111111111111111;
  return ((pmask << 12) & padd) | (vmask & vadd);
}

const size_t small_test_size = 5;
const size_t big_test_size = 1000;

int main() {
  std::cout << "========== RADIX TREE ==========" << std::endl;
  std::random_device device;
  std::mt19937 generator(device());
  std::uniform_int_distribution<uint64_t>
    dist(0, std::numeric_limits<uint64_t>::max());
  {
    std::cout << "========== SMALL TEST ==========" << std::endl;
    RadixTree rt;
  
    for(uint32_t i = 0; i < small_test_size; ++i) {
      uint64_t vadd = correctify_vadd(dist(generator));
      uint64_t padd = correctify_padd(vadd, dist(generator));
      std::cout << "INSERT(" << vadd << "," << padd << ")" << std::endl;
      bitset<64> vadd_bits(vadd);
      bitset<64> padd_bits(padd);
      std::cout << "vadd: " << vadd_bits << std::endl
                << "padd: " << padd_bits << std::endl;
      rt.insert(vadd, padd);
      RadixTreeIterator rti = rt.find(vadd);
      if(!rti.isValid()) {
        std::cout << "FIND RETURNED INVALID RESULT" << std::endl;
        return -1;
      } else {
        std::cout << "  FIND(" << vadd << "," << padd << ")" << std::endl;
      }
    }
    std::cout << "  SIZE(): " << rt.size() << std::endl
              << " CLEAR()" << std::endl;
    rt.clear();
    std::cout << "  SIZE(): " << rt.size() << std::endl;
  }
  {
    std::cout << "========== BIG TEST ==========" << std::endl;
    RadixTree rt;
    for(uint32_t i = 0; i < big_test_size; ++i) {
      uint64_t vadd = correctify_vadd(dist(generator));
      uint64_t padd = correctify_padd(vadd, dist(generator));
      rt.insert(vadd, padd);
      RadixTreeIterator rti = rt.find(vadd);
      if(!rti.isValid()) {
        std::cout << "FIND RETURNED INVALID RESULT" << std::endl;
        return -1;
      }
    }
    rt.clear();
  }
  
  return 0;
}
