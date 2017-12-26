#include "escript.h"
#include <sys/time.h>
#include <assert.h>

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
				if (!script_pause)
					++e->pass_index;
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
	if (!script_pause)
		++e->pass_index;
	return (0);
}

uint64_t print_func(struct expr_struct *e)
{
	if (e->pass_index > 0)
		return (0);
	
	uint64_t value = get_value(&e->param[0]);
	if (!script_pause)
	{
		++e->pass_index;
		printf("%s %s: %lu\n", __FUNCTION__, e->key->name, value);
	}
	
	return (0);
}

uint64_t lt_func(struct expr_struct *e)
{
//pass_index:
// 0：还未执行
// 1: value1已出
// 2: value2已出
	switch (e->pass_index)
	{
		case 0:
			e->value[1] = get_value(&e->param[0]);
			if (script_pause)
				return (0);
			e->pass_index = 1;
		case 1:
			e->value[2] = get_value(&e->param[1]);
			if (script_pause)
				return (0);
			e->pass_index = 2;			
			break;
	}
	if (e->value[1] < e->value[2])
		return 1;
	return (0);
}
uint64_t le_func(struct expr_struct *e)
{
//pass_index:
// 0：还未执行
// 1: value1已出
// 2: value2已出
	switch (e->pass_index)
	{
		case 0:
			e->value[1] = get_value(&e->param[0]);
			if (script_pause)
				return (0);
			e->pass_index = 1;			
		case 1:
			e->value[2] = get_value(&e->param[1]);
			if (script_pause)
				return (0);
			e->pass_index = 2;			
			break;
	}
	if (e->value[1] <= e->value[2])
		return 1;
	return (0);
}
uint64_t gt_func(struct expr_struct *e)
{
//pass_index:
// 0：还未执行
// 1: value1已出
// 2: value2已出
	switch (e->pass_index)
	{
		case 0:
			e->value[1] = get_value(&e->param[0]);
			if (script_pause)
				return (0);
			e->pass_index = 1;			
		case 1:
			e->value[2] = get_value(&e->param[1]);
			if (script_pause)
				return (0);
			e->pass_index = 2;			
			break;
	}
	if (e->value[1] > e->value[2])
		return 1;
	return (0);	
}
uint64_t ge_func(struct expr_struct *e)
{
//pass_index:
// 0：还未执行
// 1: value1已出
// 2: value2已出
	switch (e->pass_index)
	{
		case 0:
			e->value[1] = get_value(&e->param[0]);
			if (script_pause)
				return (0);
			e->pass_index = 1;			
		case 1:
			e->value[2] = get_value(&e->param[1]);
			if (script_pause)
				return (0);
			e->pass_index = 2;			
			break;
	}
	if (e->value[1] >= e->value[2])
		return 1;
	return (0);
}
uint64_t eq_func(struct expr_struct *e)
{
//pass_index:
// 0：还未执行
// 1: value1已出
// 2: value2已出
	switch (e->pass_index)
	{
		case 0:
			e->value[1] = get_value(&e->param[0]);
			if (script_pause)
				return (0);
			e->pass_index = 1;			
		case 1:
			e->value[2] = get_value(&e->param[1]);
			if (script_pause)
				return (0);
			e->pass_index = 2;			
			break;
	}
	if (e->value[1] == e->value[2])
		return 1;
	return (0);
}

uint64_t and_func(struct expr_struct *e)
{
//pass_index:
// 0：还未执行
// 1: value1已出
// 2: value2已出
	switch (e->pass_index)
	{
		case 0:
			e->value[1] = get_value(&e->param[0]);
			if (script_pause)
				return (0);
			e->pass_index = 1;			
		case 1:
			if (!e->value[1])
				return 0;
			e->value[2] = get_value(&e->param[1]);
			if (script_pause)
				return (0);
			e->pass_index = 2;
			break;
	}
	if (e->value[1] && e->value[1])
		return 1;
	return (0);
}

uint64_t or_func(struct expr_struct *e)
{
//pass_index:
// 0：还未执行
// 1: value1已出
// 2: value2已出
	switch (e->pass_index)
	{
		case 0:
			e->value[1] = get_value(&e->param[0]);
			if (script_pause)
				return (0);
			e->pass_index = 1;			
		case 1:
			if (e->value[1])
				return 1;
			e->value[2] = get_value(&e->param[1]);
			if (script_pause)
				return (0);
			e->pass_index = 2;
			break;
	}
	if (e->value[1] || e->value[1])
		return 1;
	return (0);
}
uint64_t not_func(struct expr_struct *e)
{
//pass_index:
// 0：还未执行
// 1: value1已出

	switch (e->pass_index)
	{
		case 0:
			e->value[1] = get_value(&e->param[0]);
			if (script_pause)
				return (0);
			e->pass_index = 1;			
			break;
	}
	if (e->value[1])
		return 0;
	return (1);
}
uint64_t if_func(struct expr_struct *e)
{
//pass_index:
// 0：还未执行cond
// 1: value1已出
	switch (e->pass_index)
	{
		case 0:
			e->value[1] = get_value(&e->param[0]);
			if (script_pause)
				return (0);
			e->pass_index = 1;			
			break;
	}

	uint64_t ret;
	if (e->value[1])
	{
		ret = call_func(&e->param[1]);
	}
	else
	{
		ret = call_func(&e->param[2]);		
	}
	return (0);
}

// uint64_t then_func(struct expr_struct *e)
// {
// 	printf("%s %d failed\n", __FUNCTION__, __LINE__);			
// 	if (e->pass_index > 0)
// 		return (0);
// 	return (0);
// }
// uint64_t else_func(struct expr_struct *e)
// {
// 	printf("%s %d failed\n", __FUNCTION__, __LINE__);				
// 	if (e->pass_index > 0)
// 		return (0);
// 	return (0);
// }
uint64_t endif_func(struct expr_struct *e)
{
	printf("%s %d failed\n", __FUNCTION__, __LINE__);				
	if (e->pass_index > 0)
		return (0);
	return (0);
}
uint64_t gethp_func(struct expr_struct *e)
{
//pass_index:
// 0：还未执行

	if (e->pass_index > 0)
		return (e->pass_index - 1);

	uint64_t ret = 10;
	printf("RUN %s\n", __FUNCTION__);
	e->pass_index = ret + 1;
	return (ret);
}
uint64_t getmoney_func(struct expr_struct *e)
{
//pass_index:
// 0：还未执行
	if (e->pass_index > 0)
		return (e->pass_index - 1);

	uint64_t ret = 100;	
	printf("RUN %s\n", __FUNCTION__);
	e->pass_index = ret + 1;	
	return (ret);
}
uint64_t submoney_func(struct expr_struct *e)
{
//pass_index:
// 0：还未执行
	if (e->pass_index > 0)
		return (e->pass_index - 1);
	
	uint64_t ret = 0;		
	printf("RUN %s\n", __FUNCTION__);	
	e->pass_index = ret + 1;	
	return (ret);
}
uint64_t addhp_func(struct expr_struct *e)
{
//pass_index:
// 0：还未执行
	if (e->pass_index > 0)
		return (e->pass_index - 1);

	uint64_t ret = 0;		
	printf("RUN %s\n", __FUNCTION__);	
	e->pass_index = ret + 1;	
	return (ret);
}
uint64_t test2_func(struct expr_struct *e)
{
//pass_index:
// 0：还未执行
	if (e->pass_index > 0)
		return (e->pass_index - 1);

	uint64_t ret = 0;		
	printf("RUN %s\n", __FUNCTION__);	
	e->pass_index = ret + 1;	
	return (ret);
}

uint64_t v1_func(struct expr_struct *e)
{
//pass_index:
// 0：还未执行
	if (e->pass_index > 0)
		return (e->pass_index - 1);

	uint64_t ret = get_value(&e->param[0]);
	if (script_pause)
		return (0);
	e->pass_index = ret + 1;		
	return ret;
}

uint64_t sleep_func(struct expr_struct *e)
{
//pass_index:
// 0：还未执行
	if (e->pass_index > 0)
		return (0);
	
	static uint64_t sleep_end_time = 0;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	if (sleep_end_time == 0)
	{
		uint64_t value = get_value(&e->param[0]);
		sleep_end_time = (tv.tv_sec + value) * 1000000 + tv.tv_usec;
		script_pause = 1;
	}
	else
	{
		uint64_t now = (tv.tv_sec) * 1000000 + tv.tv_usec;
		if (now >= sleep_end_time)
		{
			script_pause = 0;
			sleep_end_time = 0;
			e->pass_index = 1;
		}
		else
		{
			script_pause = 1;
		}
	}
	
	return (0);
}
