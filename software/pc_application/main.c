//Arturs Orbidans, for Master's thesis
//Application to process MCU requests for FPGA firmware data pages
//COM port related code is taken from: http://cygwin.com/ml/cygwin/2005-08/txt00014.txt

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "com.h"
#include "files.h"

#define io_buffer_size  60
#define com_buffer_size 600

char            io_buffer[io_buffer_size] = {0};
int             io_buffer_index = 0;
long int        file_size = 0;
//Store firmware array
unsigned char   *data_buffer;
//Store COM buffer
unsigned char com_buffer[com_buffer_size]={0};

//Parse commandline
int handle_cmd(void);

int main(int argc, char **argv)
{
    printf("---RCS MCU interface application---\n-----------------------------------\n\n");
    printf("Wait: initing COM...");
    HANDLE hCom;

    //Default com port settings
    char port_no[6] = "COM5";
    int baud_rate = 9600;   //115200

    //While not com port initialized
    while (init_com(&hCom, port_no, baud_rate) != 0) {
        printf("\nEnter COM port settings to reconnect, e.g.:\nCOM1 9600\nor press \'q\' or \'Q\' to exit:\n");
        char temp_string[20];
        fgets(temp_string, 20, stdin);
        if (temp_string[0] == 'q' || temp_string[0] == 'Q') {
            printf("\nExiting\n");
            system("pause");
            return -1;
        }
        sscanf(temp_string,"%s %d",port_no, &baud_rate);
    }

    /*
    char *pcCommPort = "COM5";
    //Get custom parameters
    short counter = 0;
    for (counter; counter<argc; counter++)
        if (strcmp(argv[counter], "com") == 0)
            pcCommPort = argv[counter+1];
    */

   int i,j,index;
   long address;
   int bytes;
   char *p;
   int request_start;

	//MAIN LOOP/////////////////////////////////////////////////////////////////

    while(1) {
        //Readkey routine
        if (_kbhit()) {
            char c = _getch();
            printf("%c",c);
            //Exit if q or Q entered
            if (c == 'q' || c == 'Q') {
                printf("\nExiting MCU interface application\n");
                system("pause");
                return 0;
            }
            io_buffer[io_buffer_index++] = (unsigned char)c;
            if (io_buffer_index == io_buffer_size)
                io_buffer_index = 0;
            //Check if end of line received
            if (c== '\r') {
                //Check if received commandline regarding FPGA reprogramming
                if (io_buffer[0] == '#') {
                    //Call specific command line function
                    handle_cmd();
                    memset(&io_buffer[0], 0, sizeof(io_buffer));
                    io_buffer_index = 0;
                    continue;
                }
                //Send to MCU
                com_write(io_buffer, io_buffer_index, &hCom);
                io_buffer_index = 0;
                //Debug
                //printf("%s\n", io_buffer);
                memset(&io_buffer[0], 0, sizeof(io_buffer));

                //Read COM and display data if any

        if (com_read(com_buffer, com_buffer_size,&hCom)<=0 || com_buffer[0]!=0) {
                printf("%s\n",com_buffer);
                memset(&com_buffer[0], 0, sizeof(com_buffer));
            }
            }
        }
    }


   while(1)
   {
    if (com_read(com_buffer, 20, &hCom) <= 0 || com_buffer[0]==0)
        continue;
    index = 0;
	while (index<com_buffer_size){
        if (com_buffer[index] == '?') {
            unsigned char *ch_ptr = (unsigned char*)&file_size;
            for (i=0; i<4; i++)
                com_write(ch_ptr+i, 1, &hCom);
            }
        if (com_buffer[index] != '#') {
            if (com_buffer[index] != 0)
                printf("%c", com_buffer[index]);
            index++;
            continue;
    } else {
            request_start = index+1;
            //printf("\nPage request received!\n");
            //printf("Req start: %d\n",request_start);
            i = 0;
            for (j=request_start; j<400; j++){
                if (com_buffer[j] == 32) i=j;
                if (com_buffer[j]==13) break;
            }
            //printf("i is %d, j is %d\n",i,j);
            p = &com_buffer[request_start];
            *(p+i-request_start) = NULL;
            address = atol(p);
            p = &com_buffer[i+1];
            *(p+j+1) = NULL;
            bytes = atol(p);
            index = j;

            if (address == 0 && bytes == 0) continue;
            printf("\n|%lu,%d| requested\n",address,bytes);
            if (address>file_size) {
                printf("EOF\n\n");
                data_buffer[0] = ',';
                com_write(data_buffer, 1, &hCom);
                return 1;
            }

            //continue; //Debug!

            //Send requested bytes to MCU via COM port
                com_write(data_buffer+address, bytes, &hCom);
    }
    };
    //Delete received array
    memset(&com_buffer[0], 0, sizeof(com_buffer));
    com_buffer[0] = 0;
    }
   free(data_buffer);
   system("pause");
   return 0;
}


int handle_cmd (void)
{
    printf("\n");
    char filename[20] = {0}; //Filename size limit!
    int i = 0;
    int j = 0;
    int counter = 0;

    bool load_firmware = false;
    bool send_to_flash = false;
    bool send_to_flash_check = false;
    bool programm_fpga = false;
    bool verify_fpga = false;
    bool crc_check = false;

    while(io_buffer[i]) {
        if (io_buffer[i] == '-') {
            switch (io_buffer[i+1]) {
            //Load firmware file
            case 'f':
                //Get filename
                load_firmware = true;
                counter = 0;
                for (j = i+3; j<io_buffer_size; j++) {
                    if (io_buffer[j] != ' ' && io_buffer[j] != '\r')
                        filename[counter++] = io_buffer[j];
                    else {
                        filename[counter] = '\0';
                        break;
                    }
                }
                break;
            //Send firmware to flash
            case 'l':
                send_to_flash = true;
                break;
            //Send firmware to flash with write check
            case 'L':
                send_to_flash_check = true;
                break;
            //Programm FPGA
            case 'p':
                programm_fpga = true;
                break;
            //Verify
            case 'v':
                verify_fpga = true;
                break;
            //Do CRC check before programming fpga
            case 'c':
                crc_check = true;
                break;
            default:
                break;
            }
        }
        i++;
        }

    //Execute passed commands in correct order///////////////////////////////
    if (load_firmware) {
        //Copy FPGA firmware file into RAM:
        if (open_file(filename) !=0 ) {
        return -1;
        }
        file_size = get_file_size();

        if (file_size <=0) {
            printf("Empty file!\n");
            return -1;
        }

        unsigned char*  buffer = (unsigned char*) malloc (sizeof(unsigned char)*file_size);
        if (copy_to_buffer(buffer, file_size) != 0) {
            return -1;
        }

            close_file();
    }

    if (send_to_flash_check) {
            //Send firmware to flash with write check
    } else if (send_to_flash) {
            //Send firmware to flash without write check
    }

    if (programm_fpga) {
        //Do firmware CRC check before programming?
        if (crc_check) {
        }

        //Verify fpga after programming it?
        if (verify_fpga){
        }
    //Perform ony fpga verify w/o programming it
    } else if (verify_fpga) {
    }
}
