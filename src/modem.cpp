#include "include/modem.h"

Modem::Modem() {
  ModemInitialize();
}

Modem::Modem(int local_address, int remote_address) {
  local_address_ = local_address;
  remote_address_ = remote_address;
}

void Modem::ModemInitialize() {
  // Enter MMPD mode
  // Sets local address 
  // Prints local address

}

int Modem::GetLocalAddress() {
  return local_address_;
}

void Modem::SetLocalAddress(int input_address) {
  local_address_ = input_address;
}


/**********************************/
/********* Helper Functions ******/
/**********************************/
void Modem::get_async(char *request) {
  // Determine packet request
  // Send down serial port
  // End (do not wait for response)
}
void Modem::get_sync(char *request) {
  // Determine packet request
  // Send down serial port
  // Wait for response
  // Return response
}

void Modem::set(char *request, int value){

}

