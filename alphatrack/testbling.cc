#include <math.h>
#include <stdlib.h>
#include <iostream>
#include "bling.h"

using namespace std;

main() {
  Bling b(7,6);
  b.set_random_mode();
  for(int i = 0; i < 40; i++) {
    if(b.run()) {
      long newlights = b.get_lights();
      std::cout << "New lights are: " << newlights << std::endl;
    }
  }
}
