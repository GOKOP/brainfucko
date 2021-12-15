#include <stdbool.h>
#include "stack.h"

typedef unsigned char uchar;

// note that if newline is true then this function will print a newline at the end
// if the brainfuck program prints anything but not otherwise
int process_program(char* program, uchar* memory, uchar** mem_ptr, int mem_size, bool newline);
