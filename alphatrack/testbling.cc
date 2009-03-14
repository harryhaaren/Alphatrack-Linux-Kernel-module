#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <vector>
#include "bling.h"

using namespace std;

typedef vector<long> arr;

struct BlingColorCmp {
  bool operator()(BlingColors b1, BlingColors b2) const {
    return b1 == b2 ? true: false;
  }
};

main() {
  Bling b(7,6);
  std::map< BlingColors, arr, BlingColorCmp > colored;
  arr r,g,y; 
  r.push_back(1); r.push_back(4);
  g.push_back(0); g.push_back(5);
  //y.push_back(3); y.push_back(6); y.push_back(7);
  y[0] = 1;
  y[1] = 2;
  colored[BlingGreen] = g;
  colored[BlingRed] =  r;
  colored[BlingYellow] =  y;

  //  colored[BlingYellow] =  b;
  std::cout << "Color Map: " << colored.size() << std::endl;
  std::cout << "Green Size: " << colored.count(BlingGreen) << std::endl;
  //  colored[BlingBlue] = { 3,2,1 };

  //  std::map<Bling::
  b.set_random_mode();
  for(int i = 0; i < 40; i++) {
    if(b.run()) {
      long newlights = b.get_lights();
      std::cout << "New lights are: " << newlights << std::endl;
    }
  }
}
