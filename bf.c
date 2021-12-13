#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

#define READ_CHUNK 100
#define MEMSIZE 30000

typedef unsigned char uchar;

// size contains the size of the written buffer, without bytes allocated but not written
char* load_program(char* fname, int* size) {
	FILE* in = fopen(fname, "r");
	if(!in) return NULL;

	*size = READ_CHUNK;
	char* program = malloc((*size) * sizeof(char));
	if(!program) return NULL;
	int offset = 0;
	int bytes_read = 0;

	while((bytes_read = fread(program + offset, 1, READ_CHUNK, in)) == READ_CHUNK) {
		*size += READ_CHUNK;
		offset += READ_CHUNK;
		char* new_block = realloc(program, (*size) * sizeof(char));
		if(!new_block) {
			free(program);
			return NULL;
		}
		program = new_block;
	}
	*size -= (READ_CHUNK - bytes_read);

	return program;
}

void skip_loop(char* program_start, int program_size, char** ptr) {
	// move through the program until the end of the encountered loop
	// which may contain other loops

	int loop_cnt = 0;
	do {
		if(**ptr == '[') ++loop_cnt;
		if(**ptr == ']') --loop_cnt;
		++(*ptr);
	} while(*ptr - program_start < program_size && loop_cnt > 0);

	if(loop_cnt > 0) {
		printf("ERROR: Bad syntax: '[' without a matching ']'\n");
		exit(2);
	}

	--(*ptr); // the loop above moves it past the ']'
	          // but outside this function it's moved further after switch
}

void incr_ptr(uchar* beg, uchar** ptr) {
	++(*ptr);
	if(*ptr >= beg + MEMSIZE) {
		printf("ERROR: Moved past allowed memory\n");
		exit(3);
	}
}

void decr_ptr(uchar* beg, uchar** ptr) {
	--(*ptr);
	if(*ptr < beg) {
		printf("ERROR: Moved past allowed memory\n");
		exit(3);
	}
}

void process_program(char* program, int prog_size, uchar* memory, uchar** mem_ptr) {
	stack_el* loops = NULL;
	char* prog_ptr = program;

	while(prog_ptr - program < prog_size) {
		switch(*prog_ptr) {
			case '>': 
				incr_ptr(memory, mem_ptr); break;
			case '<': 
				decr_ptr(memory, mem_ptr); break;
			case '+': 
				++(**mem_ptr); break;
			case '-': 
				--(**mem_ptr); break;
			case '.': 
				putchar(**mem_ptr); break;
			case ',': 
				**mem_ptr = getchar(); break;
			case '[':
				if(**mem_ptr) stack_push(&loops, prog_ptr);
				else skip_loop(program, prog_size, &prog_ptr);
				break;
			case ']':
				if(!loops) {
					printf("ERROR: Bad Syntax: ']' without matching '['\n");
					exit(2);
				} else if(**mem_ptr) prog_ptr = loops->val;
				else stack_pop(&loops);
				break;
			default: break;
		}
		++prog_ptr;
	}
}

int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	int bytes_read;
	char* program = load_program(argv[1], &bytes_read);
	if(program == NULL) {
		printf("Could not load the program from %s\n", argv[1]);
		exit(1);
	}

	uchar* memory = malloc(MEMSIZE * sizeof(uchar));
	if(!memory) {
		printf("Can't allocate program memory\n");
		exit(1);
	}
	memset(memory, 0, MEMSIZE);

	uchar* mem_ptr = memory;
	process_program(program, bytes_read, memory, &mem_ptr);

	return 0;
}
