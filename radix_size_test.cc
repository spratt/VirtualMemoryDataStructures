/******************************************************************************\
 * Much of this code comes from the Boost docs:
 * https://github.com/boostorg/intrusive/blob/develop/perf/tree_perf_test.cpp
 * http://www.boost.org/doc/libs/1_60_0/doc/html/intrusive/set_multiset.html
 * http://www.boost.org/doc/libs/1_60_0/doc/html/intrusive/avl_set_multiset.html
\******************************************************************************/

#include <boost/intrusive/rbtree.hpp>
#include <boost/intrusive/avltree.hpp>
#include <boost/intrusive/splaytree.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <vector>
#include <functional>
#include <cassert>
#include <random>
#include <cstdint>
#include "radixtree.hpp"

using namespace boost::intrusive;
using namespace boost::posix_time;

/******************************************************************************/

class MemoryMapping : public set_base_hook<optimize_size<false> >,
                      public avl_set_base_hook<optimize_size<false> >,
                      public bs_set_base_hook<optimize_size<false> >
{
  std::uint64_t va_;
  std::uint64_t pa_;

public:
  set_member_hook<> member_hook_;

  MemoryMapping(uint64_t va, uint64_t pa) : va_(va), pa_(pa) {}

  std::uint64_t getVA() { return va_; }
  std::uint64_t getPA() { return pa_; }

  friend bool operator< (const MemoryMapping &a, const MemoryMapping &b)
  { return a.va_ < b.va_; }
  friend bool operator> (const MemoryMapping &a, const MemoryMapping &b)
  { return a.va_ > b.va_; }
  friend bool operator== (const MemoryMapping &a, const MemoryMapping &b)
  { return a.va_ == b.va_; }  
};

// Define trees using the base hook
typedef rbtree<    MemoryMapping, compare<std::less<MemoryMapping> > >     RBTree;
typedef avltree<   MemoryMapping, compare<std::less<MemoryMapping> > >     AVLTree;
typedef splaytree< MemoryMapping, compare<std::less<MemoryMapping> > >     SplayTree;

/******************************************************************************/

template<class Iterator>
class MemoryContainer {
public:
  virtual void insert(MemoryMapping&) = 0;
  virtual Iterator end() = 0;
  virtual Iterator find(MemoryMapping&) = 0;
  virtual void clear() = 0;
  virtual std::size_t size() = 0;
};

class RBTreeContainer : public MemoryContainer<RBTree::iterator> {

  RBTree rbt_;

public:
  RBTreeContainer() : rbt_() {}
  void insert(MemoryMapping& mm) { rbt_.insert_unique(mm); }
  RBTree::iterator end() { return rbt_.end(); }
  RBTree::iterator find(MemoryMapping& mm) { return rbt_.find(mm); }
  void clear() { rbt_.clear(); }
  std::size_t size() { return rbt_.size(); }
};

class AVLTreeContainer : public MemoryContainer<AVLTree::iterator> {
  AVLTree avlt_;

public:
  AVLTreeContainer() : avlt_() {}
  void insert(MemoryMapping& mm) {avlt_.insert_unique(mm); }
  AVLTree::iterator end() { return avlt_.end(); }
  AVLTree::iterator find(MemoryMapping& mm) { return avlt_.find(mm); }
  void clear() { avlt_.clear(); }
  std::size_t size() { return avlt_.size(); }
};

class SplayTreeContainer : public MemoryContainer<SplayTree::iterator> {
  SplayTree splayt_;

public:
  SplayTreeContainer() : splayt_() {}
  void insert(MemoryMapping& mm) {splayt_.insert_unique(mm); }
  SplayTree::iterator end() { return splayt_.end(); }
  SplayTree::iterator find(MemoryMapping& mm) { return splayt_.find(mm); }
  void clear() { splayt_.clear(); }
  std::size_t size() { return splayt_.size(); }
};

class RadixTreeContainer : public MemoryContainer<RadixTreeIterator> {
  RadixTree radixt_;

public:
  RadixTreeContainer() : radixt_() {}
  void insert(MemoryMapping& mm) {radixt_.insert(mm.getVA(), mm.getPA()); }
  RadixTreeIterator end() { return radixt_.end(); }
  RadixTreeIterator find(MemoryMapping& mm) {
    return radixt_.find(mm.getVA());
  }
  void clear() { radixt_.clear(); }
  std::size_t size() { return radixt_.size(); }
};

/******************************************************************************/

template<class Iterator>
void test_insertion(MemoryContainer<Iterator> &c,
                    std::vector<MemoryMapping> &values,
                    std::size_t numRepeat) {
  ptime tini, tend;
  std::vector<double> times;
  // Insert
  {
    for( std::size_t repeat = 0, repeat_max = numRepeat
           ; repeat != repeat_max
           ; ++repeat){
      c.clear();
      tini = microsec_clock::universal_time();
      for( std::size_t i = 0, max = values.size()
             ; i != max
             ; ++i){
        c.insert(values[i]);
      }
      tend = microsec_clock::universal_time();
      times.push_back(double((tend-tini).total_nanoseconds())
                      /double(values.size()));
      //std::cout << "," << times.back();
      if(c.size() != values.size()){
        std::cerr << "    ERROR: size not consistent" << std::endl;
      }
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
        found += static_cast<std::size_t>(c.end() != c.find(values[i]));
      }
      tend = microsec_clock::universal_time();
      times.push_back(double((tend-tini).total_nanoseconds())/
                      double(values.size()));
      //std::cout << "," << times.back();
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
  c.clear();
}

/******************************************************************************/


int main() {

#ifdef NDEBUG
  const std::size_t numElem = 1000000;
  const std::size_t step = numElem / 20;
  const std::size_t numRepeat = 30;
#else
  const std::size_t numElem = 10000;
  const std::size_t step = numElem / 5;
  const std::size_t numRepeat = 4;
#endif
  
  std::random_device device;
  std::mt19937 generator(device());
  std::uniform_int_distribution<uint64_t>
    dist(0, std::numeric_limits<uint64_t>::max());
  std::vector<MemoryMapping> values;
  std::srand(0);
  
  std::cout << "nElements,Radix-Insert,Radix-Search" << std::endl;
  
  while(values.size() <= numElem) {
    for(uint64_t j = 0; j < step; ++j) {
      // Create several MemoryMapping objects, each one with a different value
      uint64_t vadd = correctify_vadd(dist(generator));
      uint64_t padd = correctify_padd(vadd, dist(generator));
      values.push_back(MemoryMapping(vadd, padd));
    }
    // Randomize the order
    std::random_shuffle(values.begin(), values.end());
    std::cout << values.size();
    {
      RadixTreeContainer radixtc;
      test_insertion(radixtc, values, numRepeat);
    }
  }
  
  return 0;
}
