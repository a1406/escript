#ifndef ESCRIPT_H
#define ESCRIPT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct key_words_struct;
struct expr_struct;
extern int push_expr_stack(struct expr_struct *expr);
extern int pop_expr_stack_num(int num);
extern struct expr_struct *pop_expr_stack();
extern struct expr_struct *get_last_expr(int index);
extern char *dump_stack(char *dump_buf);
extern int debug_mode;

#define MAX_EXPR_PRI_LEVEL 9999999

enum param_type {
    PARAM_TYPE_INT,
    PARAM_TYPE_STR,
    //    PARAM_TYPE_FUNC,
    PARAM_TYPE_VAR,
    PARAM_TYPE_EXPR,
	PARAM_TYPE_END,
};

struct param_struct
{
    enum param_type type;
    union {
        int                 index;
        uint64_t            i;
        char *              s;
        struct expr_struct *e;
    };
};

struct expr_struct
{
//    int                  n_param;
    struct param_struct *param;
	
	struct key_words_struct *key;
//	struct expr_struct *back_expr;  //规约到的expr
	bool final;   //是否终结
	int       pass_index;
};

typedef struct expr_struct *(*parse_func)(int argc, char *argv[], struct key_words_struct *keyword);
typedef uint64_t (*expr_func)(struct expr_struct *e);
struct key_words_struct
{
    char *     name;
    parse_func parse;	
    bool       use_left;  //使用之前的expr做第一个参数
    int        param_num;
    int       pri_level;  //优先级
    expr_func   expr;
    //    int        var_index;  //变量index
};
uint64_t setq_func(struct expr_struct *e);
uint64_t lt_func(struct expr_struct *e);
uint64_t le_func(struct expr_struct *e);
uint64_t gt_func(struct expr_struct *e);
uint64_t ge_func(struct expr_struct *e);
uint64_t eq_func(struct expr_struct *e);
uint64_t and_func(struct expr_struct *e);
uint64_t not_func(struct expr_struct *e);
uint64_t if_func(struct expr_struct *e);
uint64_t then_func(struct expr_struct *e);
uint64_t else_func(struct expr_struct *e);
uint64_t endif_func(struct expr_struct *e);
uint64_t gethp_func(struct expr_struct *e);
uint64_t getmoney_func(struct expr_struct *e);
uint64_t submoney_func(struct expr_struct *e);
uint64_t addhp_func(struct expr_struct *e);
uint64_t test2_func(struct expr_struct *e);
uint64_t print_func(struct expr_struct *e);
uint64_t progn_func(struct expr_struct *e);
#endif /* ESCRIPT_H */
