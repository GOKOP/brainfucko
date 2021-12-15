#ifndef STACK_H
#define STACK_H

typedef struct stack_element {
	char* val;
	struct stack_element* next;
} stack_el;

void stack_push(stack_el** top, char* val);
void stack_pop(stack_el** top);

#endif
