#include "escript.h"

#define MAX_VARS 10
uint64_t all_VARS[MAX_VARS];

static uint64_t call_func(struct param_struct *param)
{
	if (param->type != PARAM_TYPE_EXPR)
	{
		printf("call func failed\n");
		return (0);
	}
	struct expr_struct *e = param->e;
	return e->key->expr(e);
}

static uint64_t get_value(struct param_struct *param)
{
	switch (param->type)
	{
		case PARAM_TYPE_INT:
			return param->i;
		case PARAM_TYPE_VAR:
			return all_VARS[param->index];
		case PARAM_TYPE_EXPR:
			return call_func(param);
		default:
			printf("%s %d failed\n", __FUNCTION__, __LINE__);
			break;
	}
}

uint64_t setq_func(struct expr_struct *e)
{
	if (e->pass_index > 0)
		return (0);
	if (e->param[0].type != PARAM_TYPE_VAR)
	{
		printf("%s %d failed\n", __FUNCTION__, __LINE__);		
		return (0);
	}
	int index = e->param[0].index;
	all_VARS[index] = get_value(&e->param[1]);
	return (0);
}

uint64_t print_func(struct expr_struct *e)
{
	uint64_t value = get_value(&e->param[0]);
	printf("%lu\n", value);
	return (0);
}

uint64_t lt_func(struct expr_struct *e)
{
	return (0);
}
uint64_t le_func(struct expr_struct *e)
{
	return (0);
}
uint64_t gt_func(struct expr_struct *e)
{
	return (0);
}
uint64_t ge_func(struct expr_struct *e)
{
	return (0);
}
uint64_t eq_func(struct expr_struct *e)
{
	return (0);
}
uint64_t and_func(struct expr_struct *e)
{
	return (0);
}
uint64_t not_func(struct expr_struct *e)
{
	return (0);
}
uint64_t if_func(struct expr_struct *e)
{
	return (0);
}
uint64_t then_func(struct expr_struct *e)
{
	return (0);
}
uint64_t else_func(struct expr_struct *e)
{
	return (0);
}
uint64_t endif_func(struct expr_struct *e)
{
	return (0);
}
uint64_t gethp_func(struct expr_struct *e)
{
	return (0);
}
uint64_t getmoney_func(struct expr_struct *e)
{
	return (0);
}
uint64_t submoney_func(struct expr_struct *e)
{
	return (0);
}
uint64_t addhp_func(struct expr_struct *e)
{
	return (0);
}
uint64_t test2_func(struct expr_struct *e)
{
	return (0);
}
