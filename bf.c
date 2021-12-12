#include <stdio.h>
#include "stack.h"

int main() {
	stack_el* stack = NULL;
	stack_push(&stack, 6);
	stack_push(&stack, 3);
	stack_push(&stack, 1);
	stack_push(&stack, 4);
	printf("%lu ", stack->val);
	stack_pop(&stack);
	printf("%lu ", stack->val);
	stack_pop(&stack);
	printf("%lu ", stack->val);
	stack_pop(&stack);
	printf("%lu ", stack->val);
	return 0;
}
