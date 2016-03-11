/******************************************************************************\
 * Much of this code comes from the Boost docs:
 * https://github.com/boostorg/intrusive/blob/develop/perf/tree_perf_test.cpp
 * http://www.boost.org/doc/libs/1_60_0/doc/html/intrusive/set_multiset.html
 * http://www.boost.org/doc/libs/1_60_0/doc/html/intrusive/avl_set_multiset.html
\******************************************************************************/

#include <boost/intrusive/rbtree.hpp>
#include <boost/intrusive/avltree.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <vector>
#include <functional>
#include <cassert>

using namespace boost::intrusive;
using namespace boost::posix_time;

/******************************************************************************/

class MemoryMapping : public set_base_hook<optimize_size<true> >,
                      public avl_set_base_hook<optimize_size<true> >
{
  std::uint64_t va_;
  std::uint64_t pa_;

public:
  set_member_hook<> member_hook_;

  MemoryMapping(int va, int pa) : va_(va), pa_(pa) {}

  friend bool operator< (const MemoryMapping &a, const MemoryMapping &b)
  { return a.va_ < b.va_; }
  friend bool operator> (const MemoryMapping &a, const MemoryMapping &b)
  { return a.va_ > b.va_; }
  friend bool operator== (const MemoryMapping &a, const MemoryMapping &b)
  { return a.va_ == b.va_; }  
};

// Define trees using the base hook
typedef rbtree<  MemoryMapping, compare<std::less<MemoryMapping> > >     RBTree;
typedef avltree< MemoryMapping, compare<std::less<MemoryMapping> > >     AVLTree;

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

/******************************************************************************/

#ifdef NDEBUG
const std::size_t NumElem = 1000000;
const std::size_t NumRepeat = 30;
#else
const std::size_t NumElem = 10000;
const std::size_t NumRepeat = 4;
#endif

template<class Iterator>
void test_insertion(MemoryContainer<Iterator> &c,
                    const char *ContainerName,
                    std::vector<MemoryMapping> &values) {
  std::cout << "Container " << ContainerName << std::endl;
  //Prepare
  ptime tini, tend;
  {
    // Insert
    tini = microsec_clock::universal_time();
    for( std::size_t repeat = 0, repeat_max = NumRepeat
           ; repeat != repeat_max
           ; ++repeat){
      c.clear();
      for( std::size_t i = 0, max = values.size()
             ; i != max
             ; ++i){
        c.insert(values[i]);
      }
      if(c.size() != values.size()){
        std::cout << "    ERROR: size not consistent" << std::endl;
      }
    }
    tend = microsec_clock::universal_time();
    std::cout << "    Insert ns/iter: "
              << double((tend-tini).total_nanoseconds())/double(NumElem*NumRepeat)
              << std::endl;
  }
  // Search
  {
    tini = microsec_clock::universal_time();
    for( std::size_t repeat = 0, repeat_max = NumRepeat
           ; repeat != repeat_max
           ; ++repeat){
      std::size_t found = 0;
      for( std::size_t i = 0, max = values.size()
             ; i != max
             ; ++i){
        found += static_cast<std::size_t>(c.end() != c.find(values[i]));
      }
      if(found != NumElem){
        std::cout << "    ERROR: not all found, "
                  << found << " found out of " << NumElem
                  << std::endl;
      }
    }
    tend = microsec_clock::universal_time();
    std::cout << "    Search ns/iter: "
              << double((tend-tini).total_nanoseconds())/double(NumElem*NumRepeat)
              << std::endl;
  }
}

/******************************************************************************/

int main()
{
  std::vector<MemoryMapping> values;
  // Create several MemoryMapping objects, each one with a different value
  std::srand(0);
  for(int i = 0; i < NumElem; ++i)  {
    values.push_back(MemoryMapping(i, i + NumElem));
  }
  // Randomize the order
  std::random_shuffle(values.begin(), values.end());

  
  RBTreeContainer rbtc;
  test_insertion(rbtc, "Red-Black Tree", values);

  AVLTreeContainer avltc;
  test_insertion(avltc, "AVL Tree", values);
  
  return 0;
}
