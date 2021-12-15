#include "interpreter.h"

#include <stdio.h>
#include <stdlib.h>

// return values to be treated like a program retrun value (0 is good)

int skip_loop(char** ptr) {
	// move through the program until the end of the encountered loop
	// which may contain other loops

	int loop_cnt = 1;
	while(**ptr && loop_cnt > 0) {
		++(*ptr);
		if(**ptr == '[') ++loop_cnt;
		if(**ptr == ']') --loop_cnt;
	} 

	if(loop_cnt > 0) {
		printf("ERROR: Bad syntax: '[' without a matching ']'\n");
		return 2;
	}

	return 0;
}

int incr_ptr(uchar* beg, uchar** ptr, int mem_size) {
	if(*ptr >= beg + mem_size - 1) {
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

int process_program(char* program, uchar* memory, uchar** mem_ptr, int mem_size, bool newline) {
	stack_el* loops = NULL;
	char* prog_ptr = program;
	bool prints = false; // whether or not there were prints
	int ret = 0;

	while(*prog_ptr) {
		switch(*prog_ptr) {
			case '>': 
				ret = incr_ptr(memory, mem_ptr, mem_size); break;
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

