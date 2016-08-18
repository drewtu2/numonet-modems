#include "include/modem.h"

#include <iostream>

using namespace std;

int main() {

  Modem Yashar(1);
  Modem Andrew(3);

  int input;

  while (input != -1) {
    cout << "Andrew's Local Address: " << Andrew.get_local_address() << endl;
    cout << "Yashar's Local Address: " << Yashar.get_local_address() << endl;
    
    cout << "Enter new local address for Andrew: ";
    cin >> input;
    Andrew.set_local_address(input);
  }
  

  return 0;
};
