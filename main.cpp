#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include "escript.h"

static char dump_buf[10240];
static int dump_pos = 0;
int debug_mode;

struct expr_struct *parse_step(int argc, char *argv[], int pri_level);
int do_back_expr();

static int                 parse_index;
//static struct expr_struct *last_back_expr;

struct expr_struct *create_expr(struct key_words_struct *keyword, bool final)
{
    struct expr_struct *ret = (struct expr_struct *)malloc(sizeof(struct expr_struct));
    ret->param              = (struct param_struct *)malloc(sizeof(struct param_struct) * keyword->param_num);
//	ret->back_expr = last_back_expr;
	ret->key = keyword;
	ret->final = final;
	ret->pass_index = 0;
	if (keyword->param_num == 0)
		ret->final = true;
	return ret;
}

struct expr_struct *get_last_unfinal()
{
	for (int i = 0;; ++i)
	{
		struct expr_struct *t = get_last_expr(i);
		if (!t)
		{
			return NULL;
		}
		if (!t->final)
		{
			return t;
		}
	}
	return NULL;
}


struct expr_struct *v1_fun(int argc, char *argv[], struct key_words_struct *keyword)
{
	struct expr_struct *ret = create_expr(keyword, true);
	push_expr_stack(ret);	
	ret->param[0].type      = PARAM_TYPE_VAR;
	ret->param[0].index     = 0;
    return ret;
}
struct expr_struct *v2_fun(int argc, char *argv[], struct key_words_struct *keyword)
{
	struct expr_struct *ret = create_expr(keyword, true);
	push_expr_stack(ret);	
	ret->param[0].type      = PARAM_TYPE_VAR;
	ret->param[0].index     = 1;
    return ret;
}
struct expr_struct *v3_fun(int argc, char *argv[], struct key_words_struct *keyword)
{
	struct expr_struct *ret = create_expr(keyword, true);
	push_expr_stack(ret);	
	ret->param[0].type      = PARAM_TYPE_VAR;
	ret->param[0].index     = 2;
    return ret;
}

struct expr_struct *then_fun(int argc, char *argv[], struct key_words_struct *keyword)
{
	struct expr_struct *last = get_last_unfinal();
	if (!last || strcmp(last->key->name, "if") != 0)
	{
		printf("%s: parse failed\n", __FUNCTION__);
		exit(0);
	}
	
    struct expr_struct *ret = create_expr(keyword, false);
	ret->final = false;
	push_expr_stack(ret);
	return ret;
}

static struct param_struct void_vec_param = {.type = PARAM_TYPE_END};
void set_void_vec_param(struct expr_struct *expr)
{
	expr->param = &void_vec_param;
}

void set_vec_param(struct expr_struct *last)
{
	int param_num = 1;
	for (int i = 0;; ++i)
	{
		struct expr_struct *t = get_last_expr(i);
		if (!t->final)
		{
			break;
		}
		++param_num;
	}
	last->param = (struct param_struct *)malloc(sizeof(struct param_struct) * param_num);
	for (int i = 0; i < param_num - 1; ++i)
	{
		struct expr_struct *t = get_last_expr(param_num - 2 - i);
		last->param[i].type = PARAM_TYPE_EXPR;
		last->param[i].e = t;
	}
	last->param[param_num - 1].type = PARAM_TYPE_END;
	last->final = true;
	pop_expr_stack_num(param_num - 1);
}

struct expr_struct *endif_fun(int argc, char *argv[], struct key_words_struct *keyword)
{
	struct expr_struct *last = get_last_unfinal();
	if (!last)
	{
		printf("%s: %d parse failed\n", __FUNCTION__, __LINE__);
		exit(0);
	}
	if (strcmp(last->key->name, "then") == 0)
	{
		set_vec_param(last);
		last = create_expr(last->key, true);
		set_void_vec_param(last);
		push_expr_stack(last);
		do_back_expr();
	}
	else if (strcmp(last->key->name, "else") == 0)
	{
		set_vec_param(last);
	}
	else
	{
		printf("%s: %d parse failed\n", __FUNCTION__, __LINE__);
		exit(0);
	}
	
	return NULL;
}

struct expr_struct *else_fun(int argc, char *argv[], struct key_words_struct *keyword)
{
	struct expr_struct *last = get_last_unfinal();
	if (!last || strcmp(last->key->name, "then") != 0)
	{
		printf("%s: parse failed\n", __FUNCTION__);
		exit(0);
	}

	set_vec_param(last);
	// int param_num = 1;
	// for (int i = 0;; ++i)
	// {
	// 	struct expr_struct *t = get_last_expr(i);
	// 	if (!t->final)
	// 	{
	// 		break;
	// 	}
	// 	++param_num;
	// }
	// last->param = (struct param_struct *)malloc(sizeof(struct param_struct) * param_num);
	// for (int i = 0; i < param_num - 1; ++i)
	// {
	// 	struct expr_struct *t = get_last_expr(param_num - 2 - i);
	// 	last->param[i].type = PARAM_TYPE_EXPR;
	// 	last->param[i].e = t;
	// }
	// last->param[param_num - 1].type = PARAM_TYPE_END;
	// last->final = true;
	// pop_expr_stack_num(param_num - 1);

    struct expr_struct *ret = create_expr(keyword, false);
	ret->final = false;
	push_expr_stack(ret);
	return NULL;
}

struct expr_struct *comm_fun(int argc, char *argv[], struct key_words_struct *keyword)
{
    struct expr_struct *ret = create_expr(keyword, false);

	for (int i = 0; i < ret->key->param_num; ++i)
	{
		ret->param[i].type = PARAM_TYPE_EXPR;
	}
		
    if (keyword->use_left)
    {
		struct expr_struct *left = pop_expr_stack();		
        if (!left && !left->final)
        {
            printf("%s: %d[%s] do not have left, wrong\n", __FUNCTION__, parse_index - 1, keyword->name);
            exit(0);
        }

//		left->back_expr = last_back_expr;
		push_expr_stack(ret);
		push_expr_stack(left);
    }
	else
	{
		push_expr_stack(ret);
	}

//	if (keyword->param_num > 0)
//		last_back_expr = ret;

    return ret;
}

#define MAX_KEY_WORDS 1000
static struct key_words_struct keywords[MAX_KEY_WORDS] = {
    {"V1", v1_fun, false, 0, 1},
    {"V2", v2_fun, false, 0, 1},
    {"V3", v3_fun, false, 0, 1},

    {"=", comm_fun, true, 2, 30, setq_func},
    {"<", comm_fun, true, 2, 10, lt_func},
    {"<=", comm_fun, true, 2, 10, le_func},
    {">", comm_fun, true, 2, 10, gt_func},
    {">=", comm_fun, true, 2, 10, ge_func},
    {"==", comm_fun, true, 2, 10, eq_func},
    {"and", comm_fun, true, 2, 20, and_func},
    {"not", comm_fun, true, 1, 40, not_func},	

    {"if", comm_fun, false, 3, 100, if_func},
    {"then", then_fun, false, -1, 100, then_func},
    {"else", else_fun, false, -1, 100, else_func},
    {"endif", endif_fun, false, 0, 100, endif_func},

    {"gethp", comm_fun, false, 0, 1, gethp_func},
    {"getmoney", comm_fun, false, 0, 1, getmoney_func},
    {"submoney", comm_fun, false, 1, 1, submoney_func},
    {"addhp", comm_fun, false, 1, 1, addhp_func},	

    {"test2", comm_fun, false, 2, 1, test2_func},
	{"print", comm_fun, false, 1, 1, print_func},
};

static struct key_words_struct tmp_keyword = {"NUM", NULL, false, 1, 1};

int do_back_expr()
{
	int i = 0;
	for (;;)
	{
		struct expr_struct *last = get_last_expr(i);
		if (!last)
			return (-1);
		if (last->final)
		{
			++i;
			continue;
		}
		if (last->key->param_num != i)
		{
//			printf("parse failed %d, key[%s][%p] n_param[%d] != i[%d]\n", __LINE__,
//				last->key->name, last->key, last->key->param_num, i);
//			exit(0);
			return (-1);
		}
		for (int j = 0; j < last->key->param_num; ++j)
		{
			last->param[j].e = get_last_expr(i - j - 1);
		}
		last->final = true;
		if (debug_mode)
		{
			printf("do_back_expr [%s][%p]\n", last->key->name, last);
		}
		pop_expr_stack_num(i);
		i = 0;
	}
	// struct expr_struct *last = get_last_expr(0);
	// if (!last)
	// {
	// 	return -1;
	// }
	// struct expr_struct *back = last->back_expr;
	// if (!back)
	// {
	// 	return 1;
	// }

	// for (int i = back->key->param_num - 1; i >= 0; --i)
	// {
	// 	last = pop_expr_stack();
	// 	if (!last)
	// 	{
	// 		printf("%s i = %d, parse failed %d\n", __FUNCTION__, i, __LINE__);
	// 		exit(0);
	// 	}
	// 	back->param[i].e = last;
	// }
	// if (back != get_last_expr(0))
	// {
	// 	printf("%s parse failed %d\n", __FUNCTION__, __LINE__);
	// 	exit(0);
	// }
//	last_back_expr = back->back_expr;
	return (0);
}

struct expr_struct *parse_step(int argc, char *argv[], int pri_level)
{
    if (parse_index >= argc)
    {
        printf("parse_step parse step failed\n");
        exit(0);
    }
	struct expr_struct *last = get_last_unfinal();

    for (int j = 0; j < MAX_KEY_WORDS; ++j)
    {
        if (!keywords[j].name)
        {
            struct expr_struct *ret = create_expr(&tmp_keyword, true);
            ret->param->type        = PARAM_TYPE_INT;
            ret->param->i           = atoi(argv[parse_index]);
            ++parse_index;
			push_expr_stack(ret);
            return ret;
        }
        if (strcmp(argv[parse_index], keywords[j].name) == 0)
        {
			if (last && last->key->pri_level <= keywords[j].pri_level)
			{
				if (debug_mode)
				{
					printf("parse_step: last[%s][%d], keywords[%s][%d]\n", last->key->name, last->key->pri_level
						, keywords[j].name, keywords[j].pri_level);
				}
				do_back_expr();
			}
			
            ++parse_index;
            struct expr_struct *ret = keywords[j].parse(argc, argv, &keywords[j]);
			// if (ret)
			// {
			// 	ret->key               = &keywords[j];
			// }
			return ret;
        }
    }
    assert(0);
}

char *dump_expr_tobuff(struct expr_struct *expr, char *dump_buf, int *pos);
void dump_expr(struct expr_struct *expr)
{
	dump_pos = 0;
	if (dump_expr_tobuff(expr, dump_buf, &dump_pos))
		printf(dump_buf);
	return;
}	
char *dump_expr_tobuff(struct expr_struct *expr, char *dump_buf, int *pos)
{
    if (!expr)
        return NULL;
    assert(expr->key);

	if (expr->key->param_num == -1)
	{
		*pos = *pos + sprintf(&dump_buf[*pos], " (progn ");
		for (int i = 0; ; ++i)
		{
			switch (expr->param[i].type)
			{
				case PARAM_TYPE_EXPR:
					dump_expr_tobuff(expr->param[i].e, dump_buf, pos);
					break;
				case PARAM_TYPE_INT:
					*pos = *pos + sprintf(&dump_buf[*pos], " %lu", expr->param[0].i);
					break;
				case PARAM_TYPE_STR:
					*pos = *pos + sprintf(&dump_buf[*pos], " %s", expr->param[0].s);
					break;
				case PARAM_TYPE_VAR:
					*pos = *pos + sprintf(&dump_buf[*pos], " V%d", expr->param[0].index + 1);
					break;
				case PARAM_TYPE_END:
					*pos = *pos + sprintf(&dump_buf[*pos], ")");					
					return dump_buf;
			}
		}
	}

    if (expr->key->param_num == 1 && expr->param[0].type == PARAM_TYPE_INT)
    {
        *pos = *pos + sprintf(&dump_buf[*pos], " %lu", expr->param[0].i);
        return dump_buf;
    }
    if (expr->key->param_num == 1 && expr->param[0].type == PARAM_TYPE_STR)
    {
        *pos = *pos + sprintf(&dump_buf[*pos], " %s", expr->param[0].s);
        return dump_buf;
    }
    if (expr->key->param_num == 1 && expr->param[0].type == PARAM_TYPE_VAR)
    {
        *pos = *pos + sprintf(&dump_buf[*pos], " V%d", expr->param[0].index + 1);
        return dump_buf;
    }

    *pos = *pos + sprintf(&dump_buf[*pos], " (%s", expr->key->name);
    for (int i = 0; i < expr->key->param_num; ++i)
    {
        switch (expr->param[i].type)
        {
            case PARAM_TYPE_EXPR:
                dump_expr_tobuff(expr->param[i].e, dump_buf, pos);
                break;
            case PARAM_TYPE_INT:
                *pos = *pos + sprintf(&dump_buf[*pos], " %lu", expr->param[0].i);
                break;
            case PARAM_TYPE_STR:
                *pos = *pos + sprintf(&dump_buf[*pos], " %s", expr->param[0].s);
                break;
            case PARAM_TYPE_VAR:
                *pos = *pos + sprintf(&dump_buf[*pos], " V%d", expr->param[0].index + 1);
                break;
			case PARAM_TYPE_END:
				break;
        }
    }
    *pos = *pos + sprintf(&dump_buf[*pos], ")");
	return dump_buf;
}

int main(int argc, char *argv[])
{
	for (int i = 1; i < argc; ++i)
		printf("%s ", argv[i]);
    printf("\n");

	if (getenv("debug_mode"))
		debug_mode = 1;
	
    parse_index             = 1;
    struct expr_struct *ret = NULL;
    while (parse_index < argc)
		parse_step(argc, argv, MAX_EXPR_PRI_LEVEL);

	do_back_expr();

	ret = get_last_expr(0);
    dump_expr(ret);
    printf("\n");

	if (debug_mode)
	{
		printf("now dump stack\n");
		printf(dump_stack(dump_buf));
	}
    return 0;
}
