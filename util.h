#ifndef UTIL_H_
#define UTIL_H_

#include <stdlib.h>

#define checked_malloc malloc

typedef char *string;

typedef enum {FALSE = 0, TRUE = 1 } bool;

string String(char*);


/* 
 * using enum instead 
#define A_INT_EXP 0
#define A_LET_EXP 1
 */

/* print out error msg */ 
void assert(int errno);

typedef struct U_boolList_ *U_boolList;
struct U_boolList_ {
	bool head;
	U_boolList tail;
};
U_boolList U_BoolList(bool head, U_boolList tail);


//#define TRUE 1
//#define FALSE 0

/* Stack Structure Declaration Begin */
typedef struct stack_node_ *stack_node;
struct stack_node_ {
	void *key;
	stack_node last;
};
void GS_push(stack_node *plist, void *key);
void GS_pop(stack_node *plist);
void GS_empty(stack_node *plist);
void *GS_peek(stack_node *plist);
bool GS_check(stack_node list, void* key, bool (*compare)(void*, void*));
int GS_size(stack_node *plist);
//Stack Structure Declaration End
#endif
