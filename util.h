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

#endif
