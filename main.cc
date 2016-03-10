#include <boost/intrusive/rbtree.hpp>
#include <vector>
#include <functional>
#include <cassert>

using namespace boost::intrusive;

                  //This is a base hook optimized for size
class MemoryMapping : public set_base_hook<optimize_size<true> > {
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

//Define a set using the base hook that will store values in reverse order
typedef rbtree< MemoryMapping, compare<std::less<MemoryMapping> > >     RBTree;

int main()
{
   typedef std::vector<MemoryMapping>::iterator VectIt;

   //Create several MemoryMapping objects, each one with a different value
   std::vector<MemoryMapping> values;
   int naddresses = 100;
   for(int i = 0; i < naddresses; ++i)  values.push_back(MemoryMapping(i, i + naddresses));

   RBTree baseset;

   //Check that size optimization is activated in the base hook
   assert(sizeof(set_base_hook<optimize_size<true> >) == 3*sizeof(void*));
   //Check that size optimization is deactivated in the member hook
   assert(sizeof(set_member_hook<>) > 3*sizeof(void*));

   //Now insert them in the reverse order in the base hook set
   for(VectIt it(values.begin()), itend(values.end()); it != itend; ++it){
      baseset.insert_unique(*it);
   }

   //Now test sets
   {
      RBTree::iterator bit(baseset.begin());
      VectIt it(values.begin()), itend(values.end());

      //Test the objects inserted in the base hook set
      for(; it != itend; ++it, ++bit)
         if(&*bit != &*it)   return 1;
   }
   return 0;
}
