#ifndef NUMONET_MODEM_MODEM_H
#define NUMONET_MODEM_MODEM_H


#include "include/serial.h"


#define NUM_MESSAGES 			4
#define NUM_OF_MESSAGES 		4
#define MESSAGE_LENGTH 			164
#define INVALID_ADDRESS			0



#define REMOTE_SENDDATA_CMD		"remSendData"
#define PING_CMD			"Ping"
#define SNIFFERMODE_CMD			"sniffermode"
#define RTSATTEMPT_CMD			"rtsAttempts"
#define DATAACK_CMD			"dataAcks"
#define DATARETRANS_CMD			"dataRetrans"


struct modemPacket {
  char sentinel;
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
    char xid;

    modemPacket txPacket;
    modemPacket* ptxPacket;

public:
    Modem();
    Modem(char xid_);
    virtual ~Modem();

    int get_local_address();
    void set_local_address(int input_address);

    // Modem Operation Functions
    int Modem_execPing(char* msg, int msgLen);
    int Modem_execRemSendData(char* msg, int msgLen);
    int Modem_getLocalAddr(char* local_addr);
    int Modem_getSnifferMode(char* sniffer_mode);
    int Modem_setAck(char ack);
    int Modem_setRtsAttempt(char num_of_attempt);
    int Modem_setRetrans(char retrans);
    int Modem_setXid(char xid_);
    int Modem_Initialize(void);


private:
    int td_WrapSetPacket(const char* set_request, char set_data);
    int td_WrapExecPacket(const char* exec_request, char* exec_msg, char msglen);
    int td_WrapGetPacket(const char* get_request);
    int td_send(void);
    int td_notify(struct modemPacket* rx_ptr, int wait_interval, int num_of_wait);
};


#endif //defined NUMONET_MODEM-OOD-DEMO-MODEM_H
