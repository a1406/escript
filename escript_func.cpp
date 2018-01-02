#include "escript.h"

#define MAX_VARS 10
uint64_t all_VARS[MAX_VARS];

static uint64_t get_value(struct param_struct *param);
static uint64_t call_func(struct param_struct *param)
{
	if (param->type != PARAM_TYPE_EXPR)
	{
		printf("call func failed\n");
		return (0);
	}
	struct expr_struct *e = param->e;
	if (!e->key->expr)
	{
		if (e->key->param_num != 1)
		{
			printf("err %s %d\n", __FUNCTION__, __LINE__);
			return (0);
		}
		return get_value(&e->param[0]);
	}
	return e->key->expr(e);
}

static uint64_t *get_var(struct param_struct *param)
{
	if (param->type == PARAM_TYPE_VAR)
		return &all_VARS[param->index];

	if (param->type != PARAM_TYPE_EXPR)
	{
		printf("err %s %d\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	struct expr_struct *e = param->e;
	if (e->key->param_num != 0)
	{
		printf("err %s %d\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	if (e->param[0].type != PARAM_TYPE_VAR)
	{
		printf("err %s %d\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	return &all_VARS[e->param[0].index];
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

uint64_t progn_func(struct expr_struct *e)
{
	if (e->key->param_num != -1)
	{
		printf("%s %d failed\n", __FUNCTION__, __LINE__);				
		return (0);
	}
	for (int i = 0; ; ++i)
	{
		if (i < e->pass_index)
			continue;
		switch (e->param[i].type)
		{
			case PARAM_TYPE_EXPR:
				call_func(&e->param[i]);
				break;
			case PARAM_TYPE_END:
				return (0);
		}
	}
	return (0);
}

uint64_t setq_func(struct expr_struct *e)
{
	if (e->pass_index > 0)
		return (0);
	uint64_t *v = get_var(&e->param[0]);
	if (!v)
	{
		printf("%s %d failed\n", __FUNCTION__, __LINE__);		
		return (0);
	}
	*v = get_value(&e->param[1]);
	return (0);
}

uint64_t print_func(struct expr_struct *e)
{
	if (e->pass_index > 0)
		return (0);
	
	uint64_t value = get_value(&e->param[0]);
	printf("%s %s: %lu\n", __FUNCTION__, e->key->name, value);
	return (0);
}

uint64_t lt_func(struct expr_struct *e)
{
	if (e->pass_index > 0)
		return (0);
	
	uint64_t value1 = get_value(&e->param[0]);
	uint64_t value2 = get_value(&e->param[1]);
	if (value1 < value2)
		return 1;
	return (0);
}
uint64_t le_func(struct expr_struct *e)
{
	if (e->pass_index > 0)
		return (0);
	uint64_t value1 = get_value(&e->param[0]);
	uint64_t value2 = get_value(&e->param[1]);
	if (value1 <= value2)
		return 1;
	
	return (0);
}
uint64_t gt_func(struct expr_struct *e)
{
	if (e->pass_index > 0)
		return (0);
	uint64_t value1 = get_value(&e->param[0]);
	uint64_t value2 = get_value(&e->param[1]);
	if (value1 > value2)
		return 1;
	
	return (0);
}
uint64_t ge_func(struct expr_struct *e)
{
	if (e->pass_index > 0)
		return (0);
	uint64_t value1 = get_value(&e->param[0]);
	uint64_t value2 = get_value(&e->param[1]);
	if (value1 >= value2)
		return 1;
	
	return (0);
}
uint64_t eq_func(struct expr_struct *e)
{
	if (e->pass_index > 0)
		return (0);
	uint64_t value1 = get_value(&e->param[0]);
	uint64_t value2 = get_value(&e->param[1]);
	if (value1 == value2)
		return 1;
	
	return (0);
}
uint64_t and_func(struct expr_struct *e)
{
	if (e->pass_index > 0)
		return (0);
	uint64_t value1 = get_value(&e->param[0]);
	if (!value1)
		return (0);
	uint64_t value2 = get_value(&e->param[1]);
	return value2;
}
uint64_t or_func(struct expr_struct *e)
{
	if (e->pass_index > 0)
		return (0);
	uint64_t value1 = get_value(&e->param[0]);
	if (value1)
		return (0);
	uint64_t value2 = get_value(&e->param[1]);
	return value2;
}
uint64_t not_func(struct expr_struct *e)
{
	if (e->pass_index > 0)
		return (0);
	uint64_t value1 = get_value(&e->param[0]);
	return (!value1);
}
uint64_t if_func(struct expr_struct *e)
{
		// TODO: 
	if (e->pass_index > 0)
		return (0);
	uint64_t value1 = get_value(&e->param[0]);
	if (value1)
	{
		return call_func(&e->param[1]);
	}
	else
	{
		return call_func(&e->param[2]);		
	}
}
uint64_t then_func(struct expr_struct *e)
{
	printf("%s %d failed\n", __FUNCTION__, __LINE__);			
	if (e->pass_index > 0)
		return (0);
	return (0);
}
uint64_t else_func(struct expr_struct *e)
{
	printf("%s %d failed\n", __FUNCTION__, __LINE__);				
	if (e->pass_index > 0)
		return (0);
	return (0);
}
uint64_t endif_func(struct expr_struct *e)
{
	printf("%s %d failed\n", __FUNCTION__, __LINE__);				
	if (e->pass_index > 0)
		return (0);
	return (0);
}
uint64_t gethp_func(struct expr_struct *e)
{
	if (e->pass_index > 0)
		return (0);
	printf("RUN %s\n", __FUNCTION__);	
	return (10);
}
uint64_t getmoney_func(struct expr_struct *e)
{
	if (e->pass_index > 0)
		return (0);
	printf("RUN %s\n", __FUNCTION__);
	return (100);
}
uint64_t submoney_func(struct expr_struct *e)
{
	if (e->pass_index > 0)
		return (0);
	printf("RUN %s\n", __FUNCTION__);	
	return (0);
}
uint64_t addhp_func(struct expr_struct *e)
{
	if (e->pass_index > 0)
		return (0);
	printf("RUN %s\n", __FUNCTION__);	
	return (0);
}
uint64_t test2_func(struct expr_struct *e)
{
	if (e->pass_index > 0)
		return (0);
	printf("RUN %s\n", __FUNCTION__);	
	return (0);
}

uint64_t v1_func(struct expr_struct *e)
{
	return get_value(&e->param[0]);
}

