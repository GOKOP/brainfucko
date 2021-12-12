#include <stddef.h>

typedef struct stack_element {
	size_t val;
	struct stack_element* next;
} stack_el;

void stack_push(stack_el** top, size_t val);
void stack_pop(stack_el** top);
