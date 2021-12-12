#include "stack.h"
#include <stdlib.h>

void stack_push(stack_el** top, size_t val) {
	stack_el* new = malloc(sizeof(stack_el));
	new->val = val;
	new->next = *top;
	*top = new;
}

void stack_pop(stack_el** top) {
	if(!(*top)) return;
	stack_el* next = (*top)->next;
	free(*top);
	*top = next;
}
