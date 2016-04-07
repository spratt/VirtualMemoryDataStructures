#ifndef RADIXTREE_HPP
#define RADIXTREE_HPP
/******************************************************************************\
 * Radix Tree
 * MIT License
 * Copyright 2016, Simon Pratt
\******************************************************************************/

#include <vector>

using namespace std;

class RadixTreeIterator {
  uint64_t v_, p_;
  bool inv_;
  
public:
  RadixTreeIterator(uint64_t vadd, uint64_t padd, bool invalid = false) :
    v_(vadd), p_(padd), inv_(invalid) {}

  void setInvalid() { inv_ = true; }
  bool isValid() { return !inv_; }

  friend bool operator!= (const RadixTreeIterator &a,
                          const RadixTreeIterator &b)
  { return !(a == b); }

  friend bool operator==(const RadixTreeIterator &a,
                         const RadixTreeIterator &b)
  { return a.v_ == b.v_ && a.p_ == b.p_ && a.inv_ == b.inv_; }

  uint64_t operator* () { return this->p_; }
};

/******************************************************************************\
 * Radix Tree.  This implementation is designed as a mock-up of x86-64
 * page tables.  However, in a real implementation all table base
 * addresses would be page-aligned, and therefore require only 40 bits
 * to encode (since the low-order bits would just be 0).  This is
 * simply a mock-up and doesn't page-align anything, and neither does
 * it encode meta-information bits.  The purpose of this
 * implementation is simply to test the speed of virtual->physical
 * address mappings in a radix tree similar to the one used in a page table.
 * 
\******************************************************************************/

class RadixTree {
  vector< vector< vector< vector<uint64_t>* >* >* > t_;
  size_t s_;

/****************************************************************************\
 * A virtual address has the following form:
 *
 * 6  6         5  4      4         3         2         1         0
 * 3210987654321098765432109876543210987654321098765432109876543210
 * ----------------------------------------------------------------
 * 0000000000000000000000001000000010000000011000000100000000000000  <- example
 * \______________/\_______/\_______/\_______/\_______/\__________/
 *  sign extend(16)  p4(9)    p3(9)    p2(9)    p1(9)   offset(12)
 *
 * For the purpose of this mockup, we simply ignore the most
 * significant 16 bits.  The 9 bits (47:39) of the p4 key give an
 * index into the first level of page tables, stored in t_, which
 * gives a pointer to the p3 table.  The 9 bits (38:30) of the p3 key
 * give an index into this table, and so on.  The p1 table entry gives
 * another 64-bit integer, with the following form:
 *
 * 6  6         5         4         3         2         1         0
 * 3210987654321098765432109876543210987654321098765432109876543210
 * ----------------------------------------------------------------
 * |\_________/\______________________________________/\_/|||||||||
 * N  Avl.(11)      physical base address(40)           A GPDAPPURP
 * 
 * Everything is ignored except for the 40-bit physical base address
 * in bits 51:12, which is combined with the 12-bit offset given by
 * bits 11:0 of the virtual address, to give a 52-bit physical
 * address.
 *
\****************************************************************************/

  static uint16_t p4_key(uint64_t vadd) {
    //               \/- 9 bits
    uint64_t mask = 0b111111111;
    return (vadd & (mask << (9 + 9 + 9 + 12)) >> (9 + 9 + 9 + 12));
  }

  static uint16_t p3_key(uint64_t vadd) {
    //               \/- 9 bits
    uint64_t mask = 0b111111111;
    return (vadd & (mask << (9 + 9 + 12)) >> (9 + 9 + 12));
  }

  static uint16_t p2_key(uint64_t vadd) {
    //               \/- 9 bits
    uint64_t mask = 0b111111111;
    return (vadd & (mask << (9 + 12)) >> (9 + 12));
  }

  static uint16_t p1_key(uint64_t vadd) {
    //               \/- 9 bits
    uint64_t mask = 0b111111111;
    return (vadd & (mask << 12) >> 12);
  }

  static uint16_t offset(uint64_t vadd) {
    //               \/- 12 bits
    uint64_t mask = 0b111111111111;
    return vadd & mask;
  }

  static uint64_t pb_add(uint64_t padd) {
    //               \/- 40 bits
    uint64_t mask = 0b1111111111111111111111111111111111111111;
    return (padd & (mask << 12)) >> 12;
  }

  static const size_t p4_size = 512;
  static const size_t p3_size = 512;
  static const size_t p2_size = 512;
  static const size_t p1_size = 512;
  
public:
  
  RadixTree() : t_(p4_size), s_(0) {}
  void insert(uint64_t vadd, uint64_t padd) {
    uint16_t p4k = p4_key(vadd);
    uint16_t p3k = p3_key(vadd);
    uint16_t p2k = p2_key(vadd);
    uint16_t p1k = p1_key(vadd);
    if(t_[p4k] == NULL)
      t_[p4k] = new vector< vector< vector<uint64_t>* >* >(p3_size);
    if((*t_[p4k])[p3k] == NULL)
      (*t_[p4k])[p3k] = new vector< vector<uint64_t>* >(p2_size);
    if((*(*t_[p4k])[p3k])[p2k] == NULL)
      (*(*t_[p4k])[p3k])[p2k] = new vector<uint64_t>(p1_size);
    (*(*(*t_[p4k])[p3k])[p2k])[p1k] = pb_add(padd) << 12;
    ++s_;
  }
  RadixTreeIterator find(uint64_t vadd) {
    uint16_t p4k = p4_key(vadd);
    uint16_t p3k = p3_key(vadd);
    uint16_t p2k = p2_key(vadd);
    uint16_t p1k = p1_key(vadd);
    if(t_[p4k] == NULL ||
       (*t_[p4k])[p3k] == NULL ||
       (*(*t_[p4k])[p3k])[p2k] == NULL)
      return end();
    uint64_t base = (pb_add((*(*(*t_[p4k])[p3k])[p2k])[p1k]));
    uint64_t padd = offset(vadd) & (base << 12);
    return RadixTreeIterator(vadd, padd);
  }
  RadixTreeIterator end() { return RadixTreeIterator(-1, -1, true); }
  void clear() {
    for(size_t p4i = 0; p4i < p4_size; ++p4i) {
      vector< vector< vector<uint64_t>* >* >* p3 = t_[p4i];
      if(p3 == NULL) continue;
      for(size_t p3i = 0; p3i < p3_size; ++p3i) {
        vector< vector<uint64_t>* >* p2 = (*p3)[p3i];
        if(p2 == NULL) continue;
        for(size_t p2i = 0; p2i < p2_size; ++p2i) {
          vector<uint64_t>* p1 = (*p2)[p2i];
          if(p1 == NULL) continue;
          p1->clear();
          delete p1;
        }
        p2->clear();
        delete p2;
      }
      p3->clear();
      delete p3;
      t_[p4i] = NULL;
    }
    t_.clear();
    s_ = 0;
  }
  size_t size() { return s_; }
};

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

/******************************************************************************/
#endif
