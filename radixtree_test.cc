#include <iostream>
#include <vector>
#include <bitset>
#include <random>
#include <cstdint>
#include "radixtree.hpp"


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
