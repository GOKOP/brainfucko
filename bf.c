#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "stack.h"

#define READ_CHUNK 100
#define MEMSIZE 30000

typedef unsigned char uchar;

char* load_program(char* fname) {
	FILE* in = fopen(fname, "r");
	if(!in) return NULL;

	int size = READ_CHUNK;
	char* program = malloc(size * sizeof(char));
	if(!program) return NULL;
	int offset = 0;
	int bytes_read = 0;

	while((bytes_read = fread(program + offset, 1, READ_CHUNK, in)) == READ_CHUNK) {
		size += READ_CHUNK;
		offset += READ_CHUNK;
		char* new_block = realloc(program, size * sizeof(char));
		if(!new_block) {
			free(program);
			return NULL;
		}
		program = new_block;
	}
	size -= (READ_CHUNK - bytes_read);
	program[size] = 0;

	return program;
}

// return value to be treated like a program retrun value (0 is good)

int skip_loop(char** ptr) {
	// move through the program until the end of the encountered loop
	// which may contain other loops

	int loop_cnt = 0;
	do {
		if(**ptr == '[') ++loop_cnt;
		if(**ptr == ']') --loop_cnt;
		++(*ptr);
	} while(**ptr && loop_cnt > 0);

	if(loop_cnt > 0) {
		printf("ERROR: Bad syntax: '[' without a matching ']'\n");
		return 2;
	}

	--(*ptr); // the loop above moves it past the ']'
	          // but outside this function it's moved further after switch
	return 0;
}

int incr_ptr(uchar* beg, uchar** ptr) {
	if(*ptr >= beg + MEMSIZE - 1) {
		printf("ERROR: Moved past allowed memory\n");
		return 3;
	}
	++(*ptr);
	return 0;
}

int decr_ptr(uchar* beg, uchar** ptr) {
	if(*ptr < beg+1) {
		printf("ERROR: Moved past allowed memory\n");
		return 3;
	}
	--(*ptr);
	return 0;
}

// note that if newline is true then this function will print a newline at the end
// if the brainfuck program prints anything but not otherwise
int process_program(char* program, uchar* memory, uchar** mem_ptr, bool newline) {
	stack_el* loops = NULL;
	char* prog_ptr = program;
	bool prints = false; // whether or not there were prints
	int ret = 0;

	while(*prog_ptr) {
		switch(*prog_ptr) {
			case '>': 
				ret = incr_ptr(memory, mem_ptr); break;
			case '<': 
				ret = decr_ptr(memory, mem_ptr); break;
			case '+': 
				++(**mem_ptr); break;
			case '-': 
				--(**mem_ptr); break;
			case '.': 
				putchar(**mem_ptr); 
				prints = true;
				break;
			case ',': 
				**mem_ptr = getchar(); break;
			case '[':
				if(**mem_ptr) stack_push(&loops, prog_ptr);
				else ret = skip_loop(&prog_ptr);
				break;
			case ']':
				if(!loops) {
					printf("ERROR: Bad Syntax: ']' without matching '['\n");
					return 2;
				} else if(**mem_ptr) prog_ptr = loops->val;
				else stack_pop(&loops);
				break;
			default: break;
		}

		if(ret > 0) return ret;
		++prog_ptr;
	}

	if(newline && prints) printf("\n");
	return 0;
}

void run_from_file(char* fname) {
	char* program = load_program(fname);
	if(program == NULL) {
		printf("Could not load the program from %s\n", fname);
		exit(1);
	}

	uchar* memory = malloc(MEMSIZE * sizeof(uchar));
	if(!memory) {
		printf("Can't allocate program memory\n");
		exit(1);
	}
	memset(memory, 0, MEMSIZE);

	uchar* mem_ptr = memory;
	int ret = process_program(program, memory, &mem_ptr, false);
	if(ret > 0) exit(ret);
}

void repl() {
	uchar* memory = malloc(MEMSIZE * sizeof(uchar));
	if(!memory) {
		printf("Can't allocate program memory\n");
		exit(1);
	}

	uchar* mem_ptr = memory;

	char* linebuf = NULL;
	size_t bufsize = 0;

	while(true) {
		printf("%li: %i> ", mem_ptr - memory, *mem_ptr);
		getline(&linebuf, &bufsize, stdin);
		process_program(linebuf, memory, &mem_ptr, true);
	}
}

int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	if(strcmp(argv[1], "--repl") == 0) repl();
	else run_from_file(argv[1]);
	return 0;
}
