#ifndef NUMONET_MODEM_MODEM_H
#define NUMONET_MODEM_MODEM_H

#include "include/serial.h"

#define NUM_MESSAGES 4
#define NUM_OF_MESSAGES 4
#define MESSAGE_LENGTH 164

struct modemPacket {
  char sentinel = '@';
  char xid;
  char type;
  char number_parameters;
  char subsystem[NUM_MESSAGES];
  char field[NUM_MESSAGES];
  char length[NUM_MESSAGES];
  char message[NUM_OF_MESSAGES][MESSAGE_LENGTH];
};

class Modem {
  int local_address_, remote_address_;
  Serial serial_port;
  int xid;
 public:
  Modem();
  Modem(int local_address);
  ~Modem();

  int get_local_address();
  void set_local_address(int input_address);

 private:
  int Td_GetAsync(char* get_request, char ID);
  int Td_GetSync(char* get_request, char ID);
  int Td_WrapSetPacket(char* set_request, char set_data, char ID);
  int Td_WrapExecPacket(char* exec_request, char* exec_msg, char msglen, char ID);
  int Td_WrapGetPacket(char* get_request, char ID);
  int Td_Send(modemPacket* tx_ptr);
  int td_notify(struct modemPacket* rx_ptr, int wait_interval, int num_of_wait);
  
};


#endif //defined NUMONET_MODEM-OOD-DEMO-MODEM_H
