#include "include/modem.h"

#include <iostream>

using namespace std;

int main() {

  Modem *Yashar = new Modem::Modem(1, 2);
  Modem *Andrew = new Modem::Modem(3, 4);
  
  int input;

  while (input != -1) {
    cout << "Andrew's Local Address: " << Andrew->GetLocalAddress() << endl;
    cout << "Yashar's Local Address: " << Yashar->GetLocalAddress() << endl;
    
    cout << "Enter new local address for Andrew: ";
    cin >> input;
    Andrew->SetLocalAddress(input);
  }
  

  return 0;
};
