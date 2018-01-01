#include "escript.h"

struct expr_struct;

#define MAX_EXPR_STACK 1000
static int expr_stack_index;
struct expr_struct *expr_stack[MAX_EXPR_STACK + 1];

int push_expr_stack(struct expr_struct *expr)
{
	if (debug_mode)
	{
		printf("%s: %s[%p], index = %d\n", __FUNCTION__, expr->key->name, expr, expr_stack_index);
	}
	
	if (expr_stack_index >= MAX_EXPR_STACK)
		return (-1);
	expr_stack[expr_stack_index++] = expr;
	return (0);
}

struct expr_struct *get_last_expr(int index)
{
	if (expr_stack_index <= index)
		return NULL;

	return expr_stack[expr_stack_index - 1 - index];
}

int pop_expr_stack_num(int num)
{
	if (num <= 0)
		return 0;
	if (debug_mode)
		printf("%s: index = %d, num = %d\n", __FUNCTION__, expr_stack_index, num);
	if (expr_stack_index < num)
		return -1;
	expr_stack_index -= num;
	return (0);
}

struct expr_struct *pop_expr_stack()
{
	if (expr_stack_index <= 0)
		return NULL;

	if (debug_mode)
	{
		struct expr_struct *expr = get_last_expr(0);
		printf("%s: %s[%p], index = %d\n", __FUNCTION__, expr->key->name, expr, expr_stack_index);
	}
	
	return expr_stack[--expr_stack_index];
}

char *dump_stack(char *dump_buf)
{
	int pos = 0;
	for (int i = expr_stack_index - 1; i >= 0; --i)
	{
		pos += sprintf(&dump_buf[pos], "%s\n", expr_stack[i]->key->name);
	}
	return dump_buf;
}

