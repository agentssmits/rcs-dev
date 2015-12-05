#ifndef FILES_H_INCLUDED
#define FILES_H_INCLUDED
#include <stdio.h>


static FILE *firmware;

int open_file(char *file_name);
int close_file(void);
long int get_file_size(void);
int copy_to_buffer(unsigned char *buffer, long int file_size);

#endif // FILES_H_INCLUDED
