#include "com.h"
#include <windows.h>

static DWORD dwBytesRead    =0;
static DWORD dwBytesWrite   =0;

int init_com(HANDLE *handle_ptr, char *port_no, int baud_rate)
{
    printf("\nConnecting to port: %s @ baud: %d", port_no, baud_rate);
    DCB dcb={0};
    BOOL fSuccess;

    /***************************************CommTimeouts******************************************/
    COMMTIMEOUTS timeouts={0};
    timeouts.ReadIntervalTimeout=50;
    timeouts.ReadTotalTimeoutConstant=50;
    timeouts.ReadTotalTimeoutMultiplier=10;
    timeouts.WriteTotalTimeoutConstant=50;
    timeouts.WriteTotalTimeoutMultiplier=10;

    /*******************************************Handle*******************************************/
    *handle_ptr = CreateFile(
                    port_no,
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ,    // must be opened with exclusive-access
                    NULL, // no security attributes
                    OPEN_EXISTING, // must use OPEN_EXISTING
                    FILE_ATTRIBUTE_NORMAL,    // not overlapped I/O
                    NULL  // hTemplate must be NULL for comm devices
                    );

    /***************************************SET*UP*COM*PORT**************************************/
    if (*handle_ptr == INVALID_HANDLE_VALUE){
        printf ("\nCreateFile failed with error %d.\n", GetLastError());
       return 1;
    }

    if(!SetCommTimeouts(*handle_ptr, &timeouts)){
        printf("\nFailed to set timeouts!\n");
        return 1;
    }

    fSuccess = GetCommState(*handle_ptr, &dcb);

    if (!fSuccess){
        /*More Error Handling*/
        printf ("\nGetCommState failed with error %d.\n", GetLastError());
        return (2);
   }

   dcb.BaudRate = baud_rate;     // set the baud rate bija:
   dcb.ByteSize = 8;             // data size, xmit, and rcv
   dcb.Parity = NOPARITY;        // no parity bit
   dcb.StopBits = ONESTOPBIT;    // one stop bit
   fSuccess = SetCommState(*handle_ptr, &dcb);

   if (!fSuccess) {
      printf ("\nSetCommState failed. Error: %d.\n", GetLastError());
      return (3);
   }

   printf ("\nSerial port successfully configured.\n");
   return 0;
}


int com_write(unsigned char *array, int byte_count, HANDLE *handle_ptr)
{
    int i = WriteFile(*handle_ptr, array, byte_count, &dwBytesWrite, NULL);
    return i;
}

int com_read(unsigned char *array, int byte_count, HANDLE *handle_ptr)
{
    return ReadFile(*handle_ptr, array, byte_count, &dwBytesRead, NULL);
}
