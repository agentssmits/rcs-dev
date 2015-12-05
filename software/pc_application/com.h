#ifndef _COM_H
#define _COM_H

#include <windows.h>
#include <stdio.h>


static DWORD dwBytesRead;
static DWORD dwBytesWrite;

int init_com(HANDLE *handle_ptr, char *port_no, int baud_rate);
int com_write(unsigned char *array, int byte_count, HANDLE *handle_ptr);
int com_read(unsigned char *array, int byte_count, HANDLE *handle_ptr);



#endif
