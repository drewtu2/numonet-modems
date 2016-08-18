#include "include/modem.h"

Modem::Modem() {
  serial_port();
  char rbuffer[128];
  int num, loop, detected;
  unsigned int ui_err = 0;
  struct modemPacket mpkt;
  
  
  memset(rbuffer, 0, sizeof(rbuffer));
  detected = loop = 0;
  // Uart 2
  if (serial_port.configure(9600, 8, 1, 'N'); == 0) {
    serial_port.SetTimeout(0);
    
    // Read log data if it outputs...
    do {
      usleep(200 * 1000);
      num = serial_port.Read(rbuffer, sizeof(rbuffer));
      printf("Read Log: %d, %s.\r\n", num, rbuffer);
    } while (num > 0);
    // Send "+++" command and get response
    do {
      serial_port.Write(init_cmd, sizeof(init_cmd));
      usleep(200 * 1000);
      num = serial_port.Read(rbuffer, sizeof(rbuffer));
      if (strstr(rbuffer, "user:") != NULL) {
        detected = 1;
        printf("Modem has been detected.\r\n");
      }
      else {
        serial_port.ClearBuffer();
        if (tda_getLocalAddr(10, rbuffer) == 0) {
          detected = 1;
          printf("Modem has been  detected.\r\n");
        }
        else {
          printf("No Response found from Modem: %d, %s.\r\n", num, rbuffer);
          usleep(500 * 1000);
        }
      }
    } while ((++ loop <= 50) && (detected == 0));
  }
  else {
    printf("Cannot configure Uart correctly.\r\n");
    ui_err = 1;
  }
  
  if (detected == 1) {
    // Enter mmpd mode
    memset(rbuffer, 0, sizeof(rbuffer));
    Com_Send(mmpd_cmd, sizeof(mmpd_cmd));
    Com_uSleep(200 * 1000);
    num = Com_Receive(rbuffer, sizeof(rbuffer));
    if ((rbuffer[7] == mmpd_res[0]) && (rbuffer[8] == mmpd_res[1]) && (rbuffer[9] == mmpd_res[2])) {
      printf("Entered MMPD mode.....\r\n");
      td_set("rtsAttempts", 0, 7);
      Com_uSleep(100 * 1000);
      if (td_notify(&mpkt, 5, 200) == 0) {
        if (mpkt.type == 'n') {
          printf("Get response correctly from Modem.\r\n");
        }
        else {
          memset(&mpkt, 0, sizeof(struct modemPacket));
          td_set("rtsAttempts", 0, 29);
          Com_uSleep(50 * 1000);
          td_notify(&mpkt, 5, 200);
          if (mpkt.type == 'n') {
            printf("Get response correctly from Modem.\r\n");
          }
        }
      }
      else {
        printf("td_notify error.\r\n");
        ui_err = 4;
      }
    }
    else {
      printf("Cannot enter MMPD mode:\r\n");
      for (loop = 0; loop < num; loop ++) {
        printf("0x%x, ", rbuffer[loop]);
      }
      printf("\r\n");
      ui_err = 3;
    }
  }
  else {
    printf("No response from Modem.\r\n");
    ui_err = 2;
  }
  
  return ui_err;
}

Modem::Modem(int local_address) {
  serial_port();
  // Enter MMPD mode
  // Initialize XID with 0
  // Set the onboard and code version of the local address to be equal to the
  // passed value
  set_local_address(local_address);
}

Modem::~Modem() {
  // Be freee my resources!
}

int Modem::get_local_address() {
  return local_address_;
}

void Modem::set_local_address(int input_address) {
  local_address_ = input_address;
}


/**********************************/
/********* Helper Functions ******/
/**********************************/

int Modem::Td_WrapSetPacket(char* set_request, char set_data, char ID){
  modemPacket tx_packet;
  int error = 0;
  
  tx_packeT.xid = id;
  txpacket.type = 's';
  
  if (strcmp(set_request, "localAddress") == 0) {
    std::cout << "td_set, Command: localAddress." << std::endl;
      tx_packet.number_parameters = 1;
      tx_packet.subsystem[0] = 1;
      tx_packet.field[0] = 18;
      tx_packet.length[0] = 2;
      tx_packet.message[0][0] = 0x0;
      tx_packet.message[0][1] = set_data;
  }
  else if (strcmp(set_request, "remoteAddress") == 0) {
    std::cout << "td_set, Command: remoteAddress" << std::endl;
      tx_packet.number_parameters = 1;
      tx_packet.subsystem[0] = 1;
      tx_packet.field[0] = 14;
      tx_packet.length[0] = 2;
      tx_packet.message[0][0] = 0x0;
      tx_packet.message[0][1] = set_data;
  }
  else if (strcmp(set_request, "rtsAttempts") == 0) {
      std::cout <<"td_set, Command: rtsAttempts." << std::endl;
      tx_packet.number_parameters = 1;
      tx_packet.subsystem[0] = 67;
      tx_packet.field[0] = 0;
      tx_packet.length[0] = 6;
      tx_packet.message[0][3] = set_data;
  }
  else if (strcmp(set_request, "dataAcks") == 0) {
      std::cout <<"td_set, Command: dataAcks." << std::endl;
      tx_packet.number_parameters = 1;
      tx_packet.subsystem[0] = 67;
      tx_packet.field[0] = 13;
      tx_packet.length[0] = 6;
      tx_packet.message[0][3] = set_data;
      tx_packet.message[0][5] = 3;
  }
  else if (strcmp(set_request, "dataRetrans") == 0) {
      std::cout <<"td_set, Command: dataRetrans." << std::endl;
      tx_packet.number_parameters = 1;
      tx_packet.subsystem[0] = 1;
      tx_packet.field[0] = 2;
      tx_packet.length[0] = 2;
      tx_packet.message[0][1] = set_data;
  }
  else if (strcmp(set_request, "snifferMode") == 0) {
      std::cout <<"td_set, Command: snifferMode." << std::endl;
      tx_packet.number_parameters = 1;
      tx_packet.subsystem[0] = 72;
      tx_packet.field[0] = 1;
      tx_packet.length[0] = 6;
      tx_packet.message[0][1] = 0x01;
      tx_packet.message[0][5] = set_data;
  }
  else {
      std::cout <<"td_set, Command: It's unknown......." << std::endl;
      error = 1;
  }
  // Send command
  if (error = 0) {
      error = Td_Send(&tx_packet);
  }
  
  return error;
}

///////////////////////////////////////////////////////////////////////////
//
// Function Name:	td_send
//
// Description:		It's used to send data to the modem
//
// Parameter:
//        INPUT:
//                      tx_ptr:		Buffer data for sending
//       OUTPUT:
//                      None
//
// Return Value:
//                      0:		OK
//			else:		Error
//
//////////////////////////////////////////////////////////////////////////
int Modem::Td_Send(modemPacket* tx_ptr)
{
  int error = 0;
  int total_length;
  char tx_bytes[128];
  char* cur_ptr;
  
  tx_bytes[0] = '@';
  tx_bytes[1] = tx_ptr->xid;
  tx_bytes[2] = tx_ptr->type;
  tx_bytes[3] = tx_ptr->number_parameters;
  
  cur_ptr = &tx_bytes[4];
  for (int i = 0; i < tx_ptr->number_parameters; i ++) {
    *cur_ptr++ = tx_ptr->subsystem[i];
    *cur_ptr++ = tx_ptr->field[i];
    if (tx_ptr->type != 'g') {
      *cur_ptr++ = (char)((tx_ptr->length[i]) >> 8);
      *cur_ptr++ = (char)(tx_ptr->length[i]);
      memcpy(cur_ptr, tx_ptr->message[i], tx_ptr->length[i]);
      cur_ptr+= tx_ptr->length[i];
    }
    else {
      cur_ptr += 2;
    }
  }
  total_length = cur_ptr - tx_bytes;
  
  //For debug
  std::cout << "Send data to Modem: ";
  for (i = 0; i < total_length; i ++) {
    std::cout << "0x" << tx_bytes[i] << " ";
  }
  std::cout << std::endl;
  
  // Send command over Uart
  serial_port.Write(tx_bytes, total_length);
  
  return error;
}

///////////////////////////////////////////////////////////////////////////
//
// Function Name:       td_exec
//
// Description:         It's used to run command in the Modem
//
// Parameter:
//        INPUT:
//                      exec_request:	Request field for Modem
//                      exec_msg:	The message field
//                      msglength:         The length of message
//                      ID:		XID for Modem
//       OUTPUT:
//                      NONE
//
// Return Value:
//                      0:              OK
//                      else:           Error
//
//////////////////////////////////////////////////////////////////////////
int Modem::Td_WrapExecPacket(char* exec_request, char* exec_msg, char message_length, char ID)
{
  int error = 0;
  modemPacket exec_packet;
  
  exec_packet.xid = ID;
  exec_packet.type = 'x';
  exec_packet.number_parameters = 1;
  if (strcmp(exec_request, "remRange") == 0) {
    std::cout <<"td_exec, Command: remRange." << std::endl;
    exec_packet.subsystem[0] = 2;
    exec_packet.field[0] = 1;
    exec_packet.length[0] = msglength;
  }
  else if (strcmp(exec_request, "remTestLink") == 0) {
    std::cout <<"td_exec, Command: remTestLink." << std::endl;
    exec_packet.subsystem[0] = 1;
    exec_packet.field[0] = 8;
    exec_packet.length[0] = msglength;
  }
  else if (strcmp(exec_request, "remGetBatt") == 0) {
    std::cout <<"td_exec, Command: remGetBatt." << std::endl;
    exec_packet.subsystem[0] = 1;
    exec_packet.field[0] = 11;
    exec_packet.length[0] = msglength;
  }
  else if (strcmp(exec_request, "remMMP") == 0) {
    std::cout <<"td_exec, Command: remMMP." << std::endl;
    exec_packet.subsystem[0] = 1;
    exec_packet.field[0] = 1;
    exec_packet.length[0] = msglength;
  }
  else if (strcmp(exec_request, "remSetPower") == 0) {
    std::cout <<"td_exec, Command: remSetPower." << std::endl;
    exec_packet.subsystem[0] = 1;
    exec_packet.field[0] = 1;
    exec_packet.length[0] = msglength;
  }
  else if (strcmp(exec_request, "remGetSreg") == 0) {
    std::cout <<"td_exec, Command: remGetSreg." << std::endl;
    exec_packet.subsystem[0] = 7;
    exec_packet.field[0] = 1;
    exec_packet.length[0] = msglength;
  }
  else if (strcmp(exec_request, "remSendData") == 0) {
    std::cout <<"td_exec, Command: remSendData." << std::endl;
    exec_packet.subsystem[0] = 1;
    exec_packet.field[0] = 7;
    exec_packet.length[0] = msglength;
  }
  else if (strcmp(exec_request, "Ping") == 0) {
    std::cout <<"td_exec, Command: Ping." << std::endl;
    exec_packet.subsystem[0] = 3;
    exec_packet.field[0] = 0;
    exec_packet.length[0] = msglength;
  }
  else {
    std::cout <<"td_exec, Command: It's unknown......." << std::endl;
    error = 1;
  }
  
  // Send command
  if (error == 0) {
    error = Td_Send(&exec_packet);
  }
  
  return error;
}


///////////////////////////////////////////////////////////////////////////
//
// Function Name:       td_notify
//
// Description:         It's used to receive data from the modem
//
// Parameter:
//        INPUT:
//			wait_interval:	wait interval for the first byte
//                                      received. The number of 1ms.
//                      num_of_wait:	The number of wait_interval * 1ms for
//					the data from Modem
//       OUTPUT:
//                      rx_ptr:         Received data from modem
//
// Return Value:
//                      0:              OK
//                      else:           Error
//
//////////////////////////////////////////////////////////////////////////
int Modem::Td_Notify(struct modemPacket* rx_ptr, int wait_interval, int num_of_wait)
{
  int error = 0;
  int loop, total_length;
  char msglength[2];
  char* cur_ptr;
  
  total_length = loop = 0;
  // First, try to receive '@' symbol
  do {
      usleep(wait_interval * 1000);
      serial_port.Read(&(rx_ptr->header), 1);
      //if (num != 0) {
      //    std::cout <<"Number is %dBytes, Header is 0x%x\r\n", num, rx_ptr->header);
      //}
  } while ((rx_ptr->header != '@') && (++loop <= num_of_wait));
  // Then, receive the rest
  if (rx_ptr->header == '@') {
      // Wait for another 6ms to receive 3bytes from modem (Baudrate is 9600)
      usleep(6 * 1000);
      serial_port.Read(&(rx_ptr->xid), 1);
      serial_port.Read(&(rx_ptr->type), 1);
      serial_port.Read(&(rx_ptr->number_parameters), 1);
      total_length += 4;
      for (int i = 0; i < rx_ptr->number_parameters; i ++) {
          usleep(40 * 1000);
          serial_port.Read(&(rx_ptr->subsystem[i]), 1);
          serial_port.Read(&(rx_ptr->field[i]), 1);
          serial_port.Read(msglength, 2);
          rx_ptr->length[i] = ((int)msglength[0] << 8) + (int)msglength[1];
          total_length += 4;
          if (rx_ptr->length[i] != 0) {
              serial_port.Read(rx_ptr->message[i], rx_ptr->length[i]);
              total_length += rx_ptr->length[i];
          }
          else {
              memset(rx_ptr->message[i], 0, MESSAGE_length);
          }
      }
  }
  else {
      rx_ptr->header = 0x0;
      error = 1;
  }
  if (total_length != 0) {
      //For debug
      cur_ptr = (char*)rx_ptr;
      std::cout <<"Data received from Modem: total length is %dBytes.", total_length);
      for (int i = 0; i < 4; i ++) {
        std::cout <<"0x" << cur_ptr[i] << " ";
      }
      for (int i = 0; i < rx_ptr->number_parameters; i ++) {
        std::cout <<"0x" << rx_ptr->subsystem[i] << " ";
          std::cout <<"0x" << rx_ptr->field[i] << " ";
          std::cout <<"0x" << (char)(rx_ptr->length[i] >> 8) << " 0x" << (char)(rx_ptr->length[i]) << " ";
          for (int j = 0; j < rx_ptr->length[i]; j ++) {
              std::cout << "0x" << rx_ptr->message[i][j] << " ";
          }
      }
      std::cout << std::endl;
      total_received += total_length;
      //std::cout <<"Total Received: %d Bytes.\r\n", total_received);
  }
  
  
  return error;
}

///////////////////////////////////////////////////////////////////////////
//
// Function Name:       td_get
//
// Description:         It's used to read the parameters from the Modem
//
// Parameter:
//        INPUT:
//                      get_request:	Request field for Modem
//                      ID:		XID for Modem
//       OUTPUT:
//                      NONE
//
// Return Value:
//                      0:              OK
//                      else:           Error
//
//////////////////////////////////////////////////////////////////////////
int Modem::Td_WrapGetPacket(char* get_request, char ID)
{
  int ui_err = 0;
  modemPacket tx_packet;
  
  memset(&tx_packet, 0, sizeof(struct modemPacket));
  tx_packet.xid = ID;
  tx_packet.type = 'g';
  if (strcmp(get_request, "localAddr") == 0) {
      std::cout <<"td_get, Command: localAddr." << std::endl;
      tx_packet.number_parameters = 1;
      tx_packet.subsystem[0] = 1;
      tx_packet.field[0] = 18;
      tx_packet.length[0] = 0;
  }
  else if (strcmp(get_request, "remAddr") == 0) {
      std::cout <<"td_get, Command: remAddr." << std::endl;
      tx_packet.number_parameters = 1;
      tx_packet.subsystem[0] = 1;
      tx_packet.field[0] = 14;
      tx_packet.length[0] = 0;
  }
  else if (strcmp(get_request, "mmpStat") == 0) {
      std::cout <<"td_get, Command: mmpStat." << std::endl;
      tx_packet.number_parameters = 1;
      tx_packet.subsystem[0] = 0;
      tx_packet.field[0] = 0;
      tx_packet.length[0] = 0;
  }
  else if (strcmp(get_request, "rtsAttempts") == 0) {
      std::cout <<"td_get, Command: rtsAttempts." << std::endl;
      tx_packet.number_parameters = 1;
      tx_packet.subsystem[0] = 67;
      tx_packet.field[0] = 0;
      tx_packet.length[0] = 0;
  }
  else if (strcmp(get_request, "dataRetrans") == 0) {
      std::cout <<"td_get, Command: dataRetrans." << std::endl;
      tx_packet.number_parameters = 1;
      tx_packet.subsystem[0] = 1;
      tx_packet.field[0] = 2;
      tx_packet.length[0] = 0;
  }
  else if (strcmp(get_request, "sniffermode") == 0) {
      std::cout <<"td_get, Command: sniffermode." << std::endl;
      tx_packet.number_parameters = 1;
      tx_packet.subsystem[0] = 72;
      tx_packet.field[0] = 1;
      tx_packet.length[0] = 0;
  }
  else {
      std::cout <<"td_get, Command: It's unknown......." << std::endl;
      error = 1;
  }
  // Send command
  if (error == 0) {
      error = td_send(&tx_packet);
  }
  
  return error;
}

int Modem::Td_GetAsync(char* get_request, char ID){
  int error = 0;
  
  error = Td_WrapGetPacket(get_request, ID);
  
  return error;
}
int Modem::Td_GetSync(char* get_request, char ID){
  int error = 0;
  modemPacket* response;
  
  error = Td_WrapGetPacket(get_request, ID);
  error = Td_Notify(response, 1, 100);
  
  return error;
}




