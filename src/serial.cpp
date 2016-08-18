

/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 University of Northeastern
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mian Tang <mian.tang.neu@gmail.com>
 */



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

#include "serial.h"







pthread_mutex_t Serial::serial_lock;

int Serial::lock_flag = 0;
int Serial::uart_handle = 0;


Serial::Serial()
{
    const char path[] = "/dev/ttyO2";

    memcpy(device_path, path, sizeof(path));
    if (lock_flag == 0) {
        pthread_mutex_init(&serial_lock, NULL);
        lock_flag = 1;
    }
}


Serial::Serial(char* path, int len)
{
    memcpy(device_path, path, len);
    if (lock_flag == 0) {
        pthread_mutex_init(&serial_lock, NULL);
        lock_flag = 1;
    }
}


Serial::~Serial()
{

}


int Serial::Configure(int baud, int dataBits, int stopBits, char Parity)
{
    const int speed_arr[] = {B115200, B57600, B38400, B19200, B9600, B4800};
    const int name_arr[6] = {115200, 57600, 38400, 19200, 9600, 4800};
    struct termios opt;
    int i ,err = 0;


    pthread_mutex_lock(&serial_lock);
    if (uart_handle == 0) {
        printf("The device is %s\r\n", device_path);
        uart_handle = open(device_path, O_RDWR);
    }
    if (uart_handle >= 0) {
        if(tcgetattr(uart_handle, &opt) == -1) {
            printf("Get Uart Attribute Error.\r\n");
            err = -2;
        } 
        else {
            // Configure Baud rate
            for (i = 0; i < 6; i ++) {
                if (baud == name_arr[i]) {
                    tcflush(uart_handle, TCIOFLUSH);
                    cfsetispeed(&opt, speed_arr[i]);
                    cfsetospeed(&opt, speed_arr[i]);
                    break;
                }
            }
            // Configure Parity, Stop bit, Data bit
            opt.c_cflag &= ~CSIZE;
            switch (dataBits) {
                case 5:
                    opt.c_cflag |= CS5;
                    break;
                case 6:
                    opt.c_cflag |= CS6;
                    break;
                case 7:
                    opt.c_cflag |= CS7;
                    break;
                case 8:
                    opt.c_cflag |= CS8;
                    break;
                default:
                    printf("Unsupported data bits\n");
                    err = -4;
            }
            switch (Parity) {
                case 'n':
                case 'N':
                    opt.c_cflag &= ~PARENB;
                    opt.c_iflag &= ~INPCK;
                    break;
                case 'o':
                case 'O':
                    opt.c_cflag |= PARODD | PARENB;
                    opt.c_iflag |= INPCK;
                    break;
                case 'e':
                case 'E':
                    opt.c_cflag |= (PARENB & ~PARODD);
                    opt.c_iflag |= INPCK;
                    break;
                default:
                    printf("Unsupported parity\n");
                    err = -5;
            }
            switch (stopBits) {
                case 1:
                    opt.c_cflag &= ~CSTOPB;
                    break;
                case 2:
                    opt.c_cflag &= CSTOPB;
                    break;
                default:
                    printf("Unsupporter stop bits\n");
                    err = -6;
            }
            if (tcsetattr(uart_handle, TCSANOW, &opt) == -1 ) {
                printf("Configure baudrate error.\r\n");
                err = -3;
            }
        }
    }
    else {
        printf("Open Uart port error.\r\n");
        err = -1;
    }
    pthread_mutex_unlock(&serial_lock);


    return err;
}


int Serial::SetTimeout(char num_of_100ms)
{
    int ui_err;
    struct termios opt;


    pthread_mutex_lock(&serial_lock);
    ui_err = 0;
    if (tcgetattr(uart_handle, &opt) == -1) {
        printf("read_Timeout/tcgetattr");
        ui_err = 1;
    } else {
        //RAW mode
        opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        opt.c_oflag &= ~OPOST;
        //Close IXON,IXOFF  0x11 0x13
        opt.c_iflag &= ~(IXANY | IXON | IXOFF);
        opt.c_iflag &= ~ (INLCR | ICRNL | IGNCR);
        //set read timeout, 1 means 100ms
        opt.c_cc[VTIME] = num_of_100ms;
        opt.c_cc[VMIN] = 0;
        if(tcsetattr(uart_handle, TCSANOW, &opt) == -1) {
            printf("read_Timeout/tcsetattr");
            ui_err = 1;
        }
    }
    pthread_mutex_unlock(&serial_lock);

    return ui_err;
}


int Serial::Write(char* data_ptr, int len)
{
    int num;

    pthread_mutex_lock(&serial_lock);
    num = write(uart_handle, data_ptr, len);
    pthread_mutex_unlock(&serial_lock);

    return num;
}


int Serial::Read(char* data_ptr, int len)
{
    int num;

    pthread_mutex_lock(&serial_lock);
    num = read(uart_handle, data_ptr, len);
    pthread_mutex_unlock(&serial_lock);

    return num;
}


int Serial::ClearBuffer(void)
{
    pthread_mutex_lock(&serial_lock);
    tcflush(uart_handle, TCIFLUSH);
    pthread_mutex_unlock(&serial_lock);

    return 0;
}


int Serial::Close()
{
    close(uart_handle);

    return 0;
}



