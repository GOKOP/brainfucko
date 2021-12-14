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

void skip_loop(char** ptr) {
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

// note that if newline is true then this function will print a newline at the end
// if the brainfuck program prints anything but not otherwise
void process_program(char* program, uchar* memory, uchar** mem_ptr, bool newline) {
	stack_el* loops = NULL;
	char* prog_ptr = program;
	bool prints = false; // whether or not there were prints

	while(*prog_ptr) {
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
				putchar(**mem_ptr); 
				prints = true;
				break;
			case ',': 
				**mem_ptr = getchar(); break;
			case '[':
				if(**mem_ptr) stack_push(&loops, prog_ptr);
				else skip_loop(&prog_ptr);
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

	if(newline && prints) printf("\n");
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
	process_program(program, memory, &mem_ptr, false);
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
