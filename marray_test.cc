#include <iostream>
#include "radixtree.hpp"
#include "marray.hpp"

int main() {
  std::cout << "Now testing marray" << std::endl;

  // marray<4, 512> a;

  // std::cout << "Instantiated!" << std::endl;
  
  // a[0] = new marray<3, 512>();

  // std::cout << "Instantiated first subarray!" << std::endl;

  Marray<4, 512> m4;

  std::cout << "Instantiated Marray<4, 512>" << std::endl;

  Marray<3, 4096> m3;

  std::cout << "Instantiated Marray<3, 4096>" << std::endl;

  Marray<2, 262144> m2;

  std::cout << "Instantiated Marray<2, 262144>" << std::endl;

  m3.set(correctify_vadd(0), correctify_padd(0, 5));

  std::cout << "Assigned m3[0] = 5" << std::endl;
  
  return 0;
}
