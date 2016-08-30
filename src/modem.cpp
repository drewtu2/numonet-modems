






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
#include "termios.h" 


#include "include/modem.h"







Modem::Modem() 
{
   ptxPacket = &txPacket;
}


Modem::Modem(char xid_)
{
    xid = xid_;
    ptxPacket = &txPacket;
}



Modem::~Modem() 
{
    // Be freee my resources!
}

int Modem::get_local_address() 
{
    return local_address_;
}

void Modem::set_local_address(int input_address) 
{
    local_address_ = input_address;
}


/**********************************/
/********* Helper Functions ******/
/**********************************/

int Modem::td_send(void) 
{
    int i, error = 0;
    int total_length;
    char tx_bytes[128];
    char* cur_ptr;
 

    memset(tx_bytes, 0, sizeof(tx_bytes)); 
    tx_bytes[0] = '@';
    tx_bytes[1] = ptxPacket->xid;
    tx_bytes[2] = ptxPacket->type;
    tx_bytes[3] = ptxPacket->number_parameters;
  
    cur_ptr = &tx_bytes[4];
    for (int i = 0; i < ptxPacket->number_parameters; i ++) {
        *cur_ptr++ = ptxPacket->subsystem[i];
        *cur_ptr++ = ptxPacket->field[i];
        if (ptxPacket->type != 'g') {
            *cur_ptr++ = (char)((ptxPacket->length[i]) >> 8);
            *cur_ptr++ = (char)(ptxPacket->length[i]);
            memcpy(cur_ptr, ptxPacket->message[i], ptxPacket->length[i]);
            cur_ptr+= ptxPacket->length[i];
        }
        else {
            cur_ptr += 2;
        }
    }
    total_length = cur_ptr - tx_bytes;
  
    //For debug
    printf("Send data to Modem:");
    for (i = 0; i < total_length; i ++) {
        printf("0x%x, ", tx_bytes[i]);
    }
    printf(".\r\n");
    // Send command over Uart
    serial_port.Write(tx_bytes, total_length);

  
    return error;
}


// td_notify....
int Modem::td_notify(struct modemPacket* rx_ptr, int wait_interval, int num_of_wait)
{
    int error = 0;
    int loop, total_length;
    char msglength[2];
    char* cur_ptr;
  
    total_length = loop = 0;
    // First, try to receive '@' symbol
    do {
        usleep(wait_interval * 1000);
        serial_port.Read(&(rx_ptr->sentinel), 1);
        //if (num != 0) {
        //    std::cout <<"Number is %dBytes, Header is 0x%x\r\n", num, rx_ptr->header);
        //}
    } while ((rx_ptr->sentinel != '@') && (++loop <= num_of_wait));
    // Then, receive the rest
    if (rx_ptr->sentinel == '@') {
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
                memset(rx_ptr->message[i], 0, MESSAGE_LENGTH);
            }
        }
    }
    else {
        rx_ptr->sentinel = 0x0;
        error = 1;
    }
    if (total_length != 0) {
        //For debug
        cur_ptr = (char*)rx_ptr;
        printf("Data received from Modem: total length is %d bytes.\r\n", total_length);
        for (int i = 0; i < 4; i ++) {
            printf("0x%x, ", cur_ptr[i]);
        }
        for (int i = 0; i < rx_ptr->number_parameters; i ++) {
            printf("0x%x, ", rx_ptr->subsystem[i]);
            printf("0x%x, ", rx_ptr->field[i]);
            printf("0x%x, ", (char)(rx_ptr->length[i] >> 8));
            printf("0x%x, ", (char)(rx_ptr->length[i]));
            for (int j = 0; j < rx_ptr->length[i]; j ++) {
                printf("0x%x, ", rx_ptr->message[i][j]);
            }
        }
        printf("\r\n");
        //total_received += total_length;
        //std::cout <<"Total Received: %d Bytes.\r\n", total_received);
    }
  
  
    return error;
}



// like "td_set" in C or matlab....
int Modem::td_WrapSetPacket(const char* set_request, char set_data) 
{
    int error = 0;

    memset(ptxPacket, 0, sizeof(struct modemPacket));
    ptxPacket->sentinel = '@';
    ptxPacket->xid = xid;
    ptxPacket->type = 's';
    if (strcmp(set_request, "localAddress") == 0) {
        std::cout << "td_set, Command: localAddress." << std::endl;
        ptxPacket->number_parameters = 1;
        ptxPacket->subsystem[0] = 1;
        ptxPacket->field[0] = 18;
        ptxPacket->length[0] = 2;
        ptxPacket->message[0][0] = 0x0;
        ptxPacket->message[0][1] = set_data;
    }
    else if (strcmp(set_request, "remoteAddress") == 0) {
        std::cout << "td_set, Command: remoteAddress" << std::endl;
        ptxPacket->number_parameters = 1;
        ptxPacket->subsystem[0] = 1;
        ptxPacket->field[0] = 14;
        ptxPacket->length[0] = 2;
        ptxPacket->message[0][0] = 0x0;
        ptxPacket->message[0][1] = set_data;
    }
    else if (strcmp(set_request, "rtsAttempts") == 0) {
        std::cout <<"td_set, Command: rtsAttempts." << std::endl;
        ptxPacket->number_parameters = 1;
        ptxPacket->subsystem[0] = 67;
        ptxPacket->field[0] = 0;
        ptxPacket->length[0] = 6;
        ptxPacket->message[0][3] = set_data;
    }
    else if (strcmp(set_request, "dataAcks") == 0) {
        std::cout <<"td_set, Command: dataAcks." << std::endl;
        ptxPacket->number_parameters = 1;
        ptxPacket->subsystem[0] = 67;
        ptxPacket->field[0] = 13;
        ptxPacket->length[0] = 6;
        ptxPacket->message[0][3] = set_data;
        ptxPacket->message[0][5] = 3;
    }
    else if (strcmp(set_request, "dataRetrans") == 0) {
        std::cout <<"td_set, Command: dataRetrans." << std::endl;
        ptxPacket->number_parameters = 1;
        ptxPacket->subsystem[0] = 1;
        ptxPacket->field[0] = 2;
        ptxPacket->length[0] = 2;
        ptxPacket->message[0][1] = set_data;
    }
    else if (strcmp(set_request, "snifferMode") == 0) {
        std::cout <<"td_set, Command: snifferMode." << std::endl;
        ptxPacket->number_parameters = 1;
        ptxPacket->subsystem[0] = 72;
        ptxPacket->field[0] = 1;
        ptxPacket->length[0] = 6;
        ptxPacket->message[0][1] = 0x01;
        ptxPacket->message[0][5] = set_data;
    }
    else {
        std::cout <<"td_set, Command: It's unknown......." << std::endl;
        error = 1;
    }
  

    return error;
}


// like td_exec in C or Matlab
int Modem::td_WrapExecPacket(const char* exec_request, char* exec_msg, char msglen)
{
    int err = 0;

    memset(ptxPacket, 0, sizeof(struct modemPacket));
    ptxPacket->sentinel = '@';
    ptxPacket->xid = xid;
    ptxPacket->type = 'x';
    ptxPacket->number_parameters = 1;
    if (strcmp(exec_request, "remRange") == 0) {
        printf("td_exec, Command: remRange.\r\n");
        ptxPacket->subsystem[0] = 2;
        ptxPacket->field[0] = 1;
        ptxPacket->length[0] = msglen;
        memcpy(ptxPacket->message[0], exec_msg, msglen);
    }
    else if (strcmp(exec_request, "remTestLink") == 0) {
        printf("td_exec, Command: remTestLink.\r\n");
        ptxPacket->subsystem[0] = 1;
        ptxPacket->field[0] = 8;
        ptxPacket->length[0] = msglen;
        memcpy(ptxPacket->message[0], exec_msg, msglen);
    }
    else if (strcmp(exec_request, "remGetBatt") == 0) {
        printf("td_exec, Command: remGetBatt.\r\n");
        ptxPacket->subsystem[0] = 1;
        ptxPacket->field[0] = 11;
        ptxPacket->length[0] = msglen;
        memcpy(ptxPacket->message[0], exec_msg, msglen);
    }
    else if (strcmp(exec_request, "remMMP") == 0) {
        printf("td_exec, Command: remMMP.\r\n");
        ptxPacket->subsystem[0] = 1;
        ptxPacket->field[0] = 1;
        ptxPacket->length[0] = msglen;
        memcpy(ptxPacket->message[0], exec_msg, msglen);
    }
    else if (strcmp(exec_request, "remSetPower") == 0) {
        printf("td_exec, Command: remSetPower.\r\n");
        ptxPacket->subsystem[0] = 1;
        ptxPacket->field[0] = 1;
        ptxPacket->length[0] = msglen;
        memcpy(ptxPacket->message[0], exec_msg, msglen);
    }
    else if (strcmp(exec_request, "remGetSreg") == 0) {
        printf("td_exec, Command: remGetSreg.\r\n");
        ptxPacket->subsystem[0] = 7;
        ptxPacket->field[0] = 1;
        ptxPacket->length[0] = msglen;
        memcpy(ptxPacket->message[0], exec_msg, msglen);
    }
    else if (strcmp(exec_request, "remSendData") == 0) {
        printf("td_exec, Command: remSendData.\r\n");
        ptxPacket->subsystem[0] = 1;
        ptxPacket->field[0] = 7;
        ptxPacket->length[0] = msglen;
        memcpy(ptxPacket->message[0], exec_msg, msglen);
    }
    else if (strcmp(exec_request, "Ping") == 0) {
        printf("td_exec, Command: Ping.\r\n");
        ptxPacket->subsystem[0] = 3;
        ptxPacket->field[0] = 0;
        ptxPacket->length[0] = msglen;
        memcpy(ptxPacket->message[0], exec_msg, msglen);
    }
    else {
        printf("td_exec, Command: It's unknown.......\r\n");
        err = 1;
    }


    return err;
}


// td_get.....
int Modem::td_WrapGetPacket(const char* get_request)
{
    int err = 0;
  
    memset(ptxPacket, 0, sizeof(struct modemPacket));
    ptxPacket->sentinel = '@';
    ptxPacket->xid = xid;
    ptxPacket->type = 'g';
    if (strcmp(get_request, "localAddr") == 0) {
        std::cout <<"td_get, Command: localAddr." << std::endl;
        ptxPacket->number_parameters = 1;
        ptxPacket->subsystem[0] = 1;
        ptxPacket->field[0] = 18;
        ptxPacket->length[0] = 0;
    }
    else if (strcmp(get_request, "remAddr") == 0) {
        std::cout <<"td_get, Command: remAddr." << std::endl;
        ptxPacket->number_parameters = 1;
        ptxPacket->subsystem[0] = 1;
        ptxPacket->field[0] = 14;
        ptxPacket->length[0] = 0;
    }
    else if (strcmp(get_request, "mmpStat") == 0) {
        std::cout <<"td_get, Command: mmpStat." << std::endl;
        ptxPacket->number_parameters = 1;
        ptxPacket->subsystem[0] = 0;
        ptxPacket->field[0] = 0;
        ptxPacket->length[0] = 0;
    }
    else if (strcmp(get_request, "rtsAttempts") == 0) {
        std::cout <<"td_get, Command: rtsAttempts." << std::endl;
        ptxPacket->number_parameters = 1;
        ptxPacket->subsystem[0] = 67;
        ptxPacket->field[0] = 0;
        ptxPacket->length[0] = 0;
    }
    else if (strcmp(get_request, "dataRetrans") == 0) {
        std::cout <<"td_get, Command: dataRetrans." << std::endl;
        ptxPacket->number_parameters = 1;
        ptxPacket->subsystem[0] = 1;
        ptxPacket->field[0] = 2;
        ptxPacket->length[0] = 0;
    }
    else if (strcmp(get_request, "sniffermode") == 0) {
        std::cout <<"td_get, Command: sniffermode." << std::endl;
        ptxPacket->number_parameters = 1;
        ptxPacket->subsystem[0] = 72;
        ptxPacket->field[0] = 1;
        ptxPacket->length[0] = 0;
    }
    else {
        std::cout <<"td_get, Command: It's unknown......." << std::endl;
        err = 1;
    }

  
    return err;
}




int Modem::Modem_execPing(char* msg, int msgLen)
{
    int err = 1;

    if (td_WrapExecPacket(PING_CMD, msg, msgLen) == 0) {
        err = td_send();
    }

    return err;
}


int Modem::Modem_execRemSendData(char* msg, int msgLen)
{
    int err = 1;

    if (td_WrapExecPacket(REMOTE_SENDDATA_CMD, msg, msgLen) == 0) {
        err = td_send();
    }

    return err;
}

int Modem::Modem_getLocalAddr(char* local_addr)
{
    int i, err = 1;
    struct modemPacket mpkt;

    *local_addr = INVALID_ADDRESS;
    if (td_WrapGetPacket("localAddr") == 0) {
        err = td_send();
        if (err == 0) {
            usleep(50 * 1000);
            if (td_notify(&mpkt, 5, 100) == 0) {
                for (i = 0; i < mpkt.number_parameters; i ++) {
                    if ((mpkt.subsystem[i] == 1) && (mpkt.field[i] == 18)) {
                        *local_addr = mpkt.message[i][1];
                        err = 0;
                    }
                }
            }
            else {
                printf("No response from Modem after sending localAddr command.\r\n");
            }
        }
        else {
            printf("send localAddr command error.\r\n");
        }
    }


    return err;
}



int Modem::Modem_getSnifferMode(char* sniffer_mode)
{
    int err = 1;
    struct modemPacket mpkt;


    if (td_WrapGetPacket(SNIFFERMODE_CMD) == 0) {
        err = td_send();
        if (err == 0) {
            usleep(50 * 1000);
            if (td_notify(&mpkt, 5, 100) == 0) {
                *sniffer_mode = mpkt.message[0][3];
                err = 0;
            }
            else {
                printf("Cannot get notify from Modem after sending sniffermode Command.\r\n");
                err = 1;
            }
        }
        else {
            printf("send sniffermode command error.\r\n");
            err = 1;
        }
    }


    return err;
}



int Modem::Modem_setRtsAttempt(char num_of_attempt)
{
    int err = 1;
    struct modemPacket mpkt;

    if (td_WrapSetPacket(RTSATTEMPT_CMD, num_of_attempt) == 0) {
        err = td_send();
        if (err == 0) {
            usleep(100 * 1000);
            if (td_notify(&mpkt, 5, 200) == 0) {
                if (mpkt.type == 'n') {
                    err = 0;
                }
            }
            else {
                printf("Cannot get notify from modem after sending rtsAttempts command.\r\n");
            }
        }
        else {
            printf("send rtsAttempts command error.\r\n");
        }
    }


    return err;
}



int Modem::Modem_setAck(char ack)
{
    int err = 1;

    if (td_WrapSetPacket(DATAACK_CMD, ack) == 0) {
        err = td_send();
        if (err != 0) {
            printf("send dataAcks command error.\r\n");
        }
    }


    return err;
}


int Modem::Modem_setRetrans(char retrans)
{
    int err = 1;

    if (td_WrapSetPacket(DATARETRANS_CMD, retrans) == 0) {
        err = td_send();
        if (err != 0) {
            printf("send dataAcks command error.\r\n");
        }
    }


    return err;
}


int Modem::Modem_setXid(char xid_)
{
    xid = xid_;

    return 0;
}


int Modem::Modem_Initialize(void)
{
    char init_cmd[] = {0x2B, 0x2B, 0x2B, 0x0A};
    char mmpd_cmd[] = {0x6D, 0x6D, 0x70, 0x64, 0x0A};
    char mmpd_res[] = {0x40, 0x00, 0x6E};
    char rbuffer[128];
    int num, loop, detected;
    int err = 0;


    memset(rbuffer, 0, sizeof(rbuffer));
    detected = loop = 0;
    // Uart 2
    if (serial_port.Configure(9600, 8, 1, 'N') == 0) {
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
                if (Modem_getLocalAddr(rbuffer) == 0) {
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
        err = 1;
    }

    if (detected == 1) {
        // Enter mmpd mode
        memset(rbuffer, 0, sizeof(rbuffer));
        serial_port.Write(mmpd_cmd, sizeof(mmpd_cmd));
        usleep(200 * 1000);
        num = serial_port.Read(rbuffer, sizeof(rbuffer));
        if ((rbuffer[7] == mmpd_res[0]) && (rbuffer[8] == mmpd_res[1]) && (rbuffer[9] == mmpd_res[2])) {
            printf("Entered MMPD mode.....\r\n");
            if (Modem_setRtsAttempt(0) != 0) {
                usleep(100 * 1000);
                if (Modem_setRtsAttempt(0) == 0) {
                    printf("Get response correctly from Modem.\r\n");
                }
                else {
                    err = 6;
                }
            }
            else {
                printf("Get response correctly from Modem.\r\n");
            }
        }
        else {
            printf("Cannot enter MMPD mode:\r\n");
            for (loop = 0; loop < num; loop ++) {
                printf("0x%x, ", rbuffer[loop]);
            }
            printf("\r\n");
            err = 3;
        }
    }
    else {
        printf("No response from Modem.\r\n");
        err = 2;
    }


    return err;
}






















