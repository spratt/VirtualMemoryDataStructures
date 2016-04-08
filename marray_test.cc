#include <boost/date_time/posix_time/posix_time.hpp>
#include <set>
#include <iostream>
#include <vector>
#include <functional>
#include <cassert>
#include <random>
#include <cstdint>
#include "radixtree.hpp"
#include "marray.hpp"

using MemoryMapping = std::pair<uint64_t, uint64_t>;
using namespace boost::posix_time;

template<std::size_t N, std::size_t M>
void test_insertion(Marray<N,M> m,
                    std::vector<MemoryMapping> &values,
                    std::size_t numRepeat) {
  ptime tini, tend;
  std::vector<double> times;
  // Insert
  std::cout << N;
  std::cout << std::fixed << std::setprecision(1);
  {
    for( std::size_t repeat = 0, repeat_max = numRepeat
           ; repeat != repeat_max
           ; ++repeat){
      m.clear();
      tini = microsec_clock::universal_time();
      for( std::size_t i = 0, max = values.size()
             ; i != max
             ; ++i){
        m.set(values[i].first, values[i].second);
      }
      tend = microsec_clock::universal_time();
      times.push_back(double((tend-tini).total_nanoseconds())
                      /double(values.size()));
      // if(c.size() != values.size()){
      //   std::cerr << "    ERROR: size not consistent" << std::endl;
      // }
    }
    double total = 0.0;
    while(!times.empty()) {
      total += times.back();
      times.pop_back();
    }
    std::cout << "," << total/numRepeat;
  }
  // Search
  {
    for( std::size_t repeat = 0, repeat_max = numRepeat
           ; repeat != repeat_max
           ; ++repeat){
      std::size_t found = 0;
      tini = microsec_clock::universal_time();
      for( std::size_t i = 0, max = values.size()
             ; i != max
             ; ++i){
        found += static_cast<std::size_t>(values[i].second ==
                                          m.get(values[i].first));
      }
      tend = microsec_clock::universal_time();
      times.push_back(double((tend-tini).total_nanoseconds())/
                      double(values.size()));
      if(found != values.size()){
        std::cerr << "    ERROR: not all found, "
                  << found << " found out of " << values.size()
                  << std::endl;
      }
    }
    double total = 0.0;
    while(!times.empty()) {
      total += times.back();
      times.pop_back();
    }
    std::cout << "," << total/numRepeat << std::endl;
  }
  m.clear();
}

int main() {

#ifdef NDEBUG
  std::size_t numElem = 1000000;
  std::size_t numRepeat = 30;
#else
  std::size_t numElem = 100;
  std::size_t numRepeat = 4;
#endif

  std::random_device device;
  std::mt19937 generator(device());
  std::uniform_int_distribution<uint64_t>
    dist(0, std::numeric_limits<uint64_t>::max());
  std::set<uint64_t> keys;
  std::vector<MemoryMapping> values;
  // Create several MemoryMapping objects, each one with a different value
  std::srand(0);
  while(keys.size() < numElem) {
    keys.insert(correctify_vadd(dist(generator)));
  }
  for(set<uint64_t>::const_iterator it = keys.cbegin();
      it != keys.cend();
      ++it) {
    uint64_t vadd = *it;
    uint64_t padd = correctify_padd(vadd, dist(generator));
    values.push_back(MemoryMapping(vadd, padd));
  }
  // Randomize the order
  std::random_shuffle(values.begin(), values.end());
  
  std::cout << "nlvls,avgins,avgfnd" << std::endl;
  {
    Marray<2, 262144> m2;
    test_insertion(m2, values, numRepeat);
  }
  {
    Marray<3, 4096> m3;
    test_insertion(m3, values, numRepeat);
  }
  {
    Marray<4, 512> m4;
    test_insertion(m4, values, numRepeat);
  }
  {
    Marray<6, 64> m6;
    test_insertion(m6, values, numRepeat);
  }
  {
    Marray<9, 16> m6;
    test_insertion(m6, values, numRepeat);
  }
  {
    Marray<12, 8> m6;
    test_insertion(m6, values, numRepeat);
  }
  {
    Marray<18, 4> m6;
    test_insertion(m6, values, numRepeat);
  }
  {
    Marray<36, 2> m6;
    test_insertion(m6, values, numRepeat);
  }
  
  // std::cout << "Now testing marray" << std::endl;

  // Marray<4, 512> m4;

  // std::cout << "Instantiated Marray<4, 512>" << std::endl;

  // Marray<3, 4096> m3;

  // std::cout << "Instantiated Marray<3, 4096>" << std::endl;

  // Marray<2, 262144> m2;

  // std::cout << "Instantiated Marray<2, 262144>" << std::endl;

  // uint64_t k = correctify_vadd(dist(generator));
  // uint64_t v = correctify_padd(k, dist(generator));
  // m3.set(k, v);
  // m3.set(k+1,v);

  // std::cout << std::hex;
  // std::cout << "Assigned m3[0x" << k << "] = 0x" << v <<  std::endl;

  // uint64_t got = m3.get(k);
  // std::cout << "Read     m3[0x" << k << "] = 0x" << got << std::endl;

  // m3.clear();
  
  // std::cout << "Cleared m3" << std::endl;
  
  // m3.set(k, v);
  // m3.set(k-1, v);
  
  // std::cout << "Assigned m3[0x" << k << "] = 0x" << v <<  std::endl;

  // got = m3.get(k);
  // std::cout << "Read     m3[0x" << k << "] = 0x" << got << std::endl;

  // m3.clear();
  
  // std::cout << "Cleared m3" << std::endl;
  
  // m3.clear();
  
  // std::cout << "Cleared m3 again" << std::endl;

  // for(size_t i = 0; i < values.size(); ++i) {
  //   m3.set(values[i].first, values[i].second);
  // }

  // std::cout << std::dec;
  // std::cout << "Inserted " << numElem << " elements into m3" << std::endl;

  // std::cout << "Getting elements...";
  // bool succeeded = true;
  // uint64_t fail_key, expected, found;
  // for(size_t i = 0; i < values.size(); ++i) {
  //   if(m3.get(values[i].first) != values[i].second) {
  //     succeeded = false;
  //     fail_key = values[i].first;
  //     expected = values[i].second;
  //     found = m3.get(values[i].first);
  //   }
  // }
  // if(succeeded) {
  //   std::cout << "success." <<std::endl;
  // } else {
  //   std::cout << std::hex;
  //   std::cout << "failure, k: " << fail_key
  //             << ", expected: " << expected
  //             << ", found: " << found
  //             << std::endl;
  // }

  // m3.clear();
  
  // std::cout << "Cleared m3" << std::endl;
  
  return 0;
}
