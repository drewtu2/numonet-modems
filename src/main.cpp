#include "include/modem.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>



using namespace std;

int main() {

  //Modem *Yashar = new Modem(1);
  //Modem *Andrew = new Modem(3);
  int vl;
  char addr;
  Modem a(10);


  vl = a.Modem_Initialize();
  printf("Modem initialize: %d.\r\n", vl);
  //a.Modem_setAck(0);
  //printf("Modem ACK: %d.\r\n", vl);
  //a.Modem_setRetrans(0);
  //printf("Modem retransmission: %d.\r\n", vl);
  a.Modem_setXid(0x31);
  a.Modem_getLocalAddr(&addr);
  printf("Modem address is 0x%x.\r\n", addr);



#if 0   // For Serial class test
  Serial serialport;
  int num;
  char buf[] = "Hello, World!";
  char data[1024];

  serialport.Configure(9600, 8, 1, 'N');
  serialport.SetTimeout(1);
  do {
      usleep(200 * 1000);
      memset(data, 0, sizeof(data));
      num = serialport.Read(data, 64);
      printf("Data read is %dBytes: %s\r\n", num, data);
  } while (1);
#endif


    
  

  return 0;
};
