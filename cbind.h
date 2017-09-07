#ifndef CBIND_H
#define CBIND_H

#define INC0 1
#define INC1 2
#define INC2 3
#define INC3 4
#define INC4 5
#define INC5 6
#define INC6 7
#define INC7 8
#define INC8 9
#define INC9 10
#define INC10 11
#define INC11 12
#define INC12 13
#define INC13 14
#define INC14 15
#define INC15 16

#define EVAL1(ARGS...) ARGS
#define EVAL2(ARGS...) EVAL1(EVAL1(ARGS))
#define EVAL4(ARGS...) EVAL2(EVAL2(ARGS))
#define EVAL8(ARGS...) EVAL4(EVAL4(ARGS))

#define EMPTY()
#define DEFER(FUNC, ARGS...) FUNC EMPTY() (ARGS)
#define DEFER2(FUNC, ARGS...) FUNC EMPTY EMPTY()() (ARGS)

#define _THIRD(ARG1, ARG2, ARG3, ...) ARG3
#define _IS_NOT_EMPTY(ARGS...) _THIRD(~, ## ARGS, TRUE, FALSE)
#define IS_NOT_EMPTY(ARG1, ARGS...) _IS_NOT_EMPTY(ARG1)

#define _IF_EVAL_COND(COND) _IF_ ## COND
#define IF_NOT_EMPTY(ARGS...) DEFER( _IF_EVAL_COND, IS_NOT_EMPTY(ARGS))
#define _IF_TRUE(ARGS...) ARGS
#define _IF_FALSE(ARGS...)

#define DECLV(I, V) __auto_type _v ## I = (V);
#define DECLS(I, V) typeof(_v ## I) _v ## I;
#define INITS(I, V) _v ## I
#define CALLA(I, V) _args->_v ## I

#define COMMA() ,

#define _MAP_PROXY() _MAP
#define _MAP(FUNC, SEP, I, V, ARGS...) FUNC(I, V)\
	IF_NOT_EMPTY(ARGS)(SEP() DEFER2(_MAP_PROXY)(FUNC, SEP, INC ## I, ARGS))
#define MAP(FUNC, SEP, ARGS...) EVAL8(_MAP(FUNC, SEP, 0, ARGS))

#define make_call(ALLOC, FUNC, ARGS...) ({\
MAP(DECLV, EMPTY, ARGS);\
struct _args {void *func; void *ret; MAP(DECLS, EMPTY, ARGS)} _args =\
	(struct _args){NULL, NULL, MAP(INITS, COMMA, ARGS)};\
asm volatile goto(\
"leaq %0, %%rcx\n"\
"leaq %l[start], %%rax\n"\
"movq %%rax, 0(%%rcx)\n"\
"leaq %l[exit], %%rax\n"\
"movq %%rax, 8(%%rcx)\n"\
"jmp %l[exit]\n"\
:\
: "m"(_args)\
: "rax", "rcx"\
: start, ret, exit);\
start:\
{\
register struct _args *_args asm ("r12");\
asm volatile (\
	"and $0xfffffffffffffff0, %%rsp\n"\
	: "=r"(_args));\
FUNC(MAP(CALLA, COMMA, ARGS));\
ret:\
asm volatile(\
	"mov 8(%0), %%rax\n"\
	"jmp *%%rax\n"\
	:\
	: "r"(_args));\
}\
exit:\
ALLOC(&_args, sizeof(_args));\
;})

static inline void
exec_call(void *call)
{
	asm volatile goto(
		"mov %0, %%r12\n"
		"leaq %l[exit], %%rax\n"
		"mov %%rax, 8(%%r12)\n"
		"mov 0(%%r12), %%rax\n"
		"jmp *%%rax\n"
	:
	: "=r" (call)
	: "r12", "rax", "rsp", "rdi", "rsi", "rcx", "rdx", "r8", "r9"
	: exit);
exit:
	return;
}
#endif
