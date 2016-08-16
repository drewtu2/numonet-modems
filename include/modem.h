#ifndef NUMONET_MODEM_MODEM_H
#define NUMONET_MODEM_MODEM_H

class Modem {
  int local_address_, remote_address_;
  
 public:
  Modem();
  Modem(int local_address, int remote_address);
  int GetLocalAddress();
  void SetLocalAddress(int input_address);
  void ModemInitialize();
 
 private:
  void get_async(char *request);
  void get_sync(char *request);
  void set(char *request, int value);
/*  void exec();
  void notify();
*/
  
};


#endif //defined NUMONET_MODEM-OOD-DEMO-MODEM_H
