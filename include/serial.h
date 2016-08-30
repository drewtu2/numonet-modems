
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

#ifndef SERIAL_H
#define SERIAL_H




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




#define MAX_PATH_LENGTH		64

/**
 *
 * A class used for the operation over UART
 *
 */
class Serial {

private:
    char device_path[MAX_PATH_LENGTH];
    int baud_rate;
    int data_bits;
    int stop_bits;
    char parity_bit;

    static pthread_mutex_t serial_lock;  
    static int lock_flag;
    static int uart_handle;


public:
    Serial();
    Serial(char* path, int len);
    virtual ~Serial();

    int Configure(int baud, int dataBits, int stopBits, char Parity);
    int SetTimeout(char num_of_100ms);
    int ClearBuffer();
    int Write(char* data_ptr, int len);
    int Read(char* data_ptr, int len);
    int Close();
};




#endif /* SERIAL_H */
