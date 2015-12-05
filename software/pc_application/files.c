#include "files.h"

static FILE *firmware;

int open_file(char *file_name)
{
    firmware = fopen(file_name, "rb");
    if (!firmware) {
        printf("Unable to open \'%s\' !\n", file_name);
        return -1;
    } else
        printf("File \'%s\' opened!\n", file_name);
        return 0;
}

int close_file()
{
    close(firmware);
}

long int get_file_size()
{
    //Get file size
    fseek(firmware, 0L, SEEK_END);
    int sz = ftell(firmware);
    fseek(firmware, 0L, SEEK_SET);
    printf("File size: %d bytes\n",sz);
    return sz;
}

int copy_to_buffer(unsigned char *buffer, long int file_size)
{
    //Allocate memory to contain the whole file:
    if (buffer == NULL) {
        fputs ("Memory error",stderr);
        return -1;
    }
    int result = fread (buffer,1,file_size, firmware);
    if (result != file_size) {
        fputs ("Reading error",stderr);
        return -1;
    }
    return 0;
}
