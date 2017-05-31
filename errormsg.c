#include "errormsg.h"
#include "util.h"
#include "log.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>


bool anyErrors = FALSE;
int EM_tokPos = 0;

static int lineNum = 1;
static string TAG = "Error Message";
typedef struct intList { 
	int i; 
	struct intList *rest; 
} *IntList;

static IntList intList(int i, IntList rest) {
	IntList l = (IntList)checked_malloc(sizeof(*l));
	l->i = i; l->rest = rest;
	return l;
}

static IntList linePos = NULL;


void EM_newline(void)
{
	lineNum++;
	linePos = intList(EM_tokPos, linePos);
}

void EM_error(int pos, string message,...)
{
	va_list ap;

	IntList lines = linePos;
	int num = lineNum;

	anyErrors = TRUE;
	while (lines && lines->i >= pos){
		lines = lines->rest; 
		num--;
	}
	if (lines)
		fprintf(stderr, "%d.%d: ", num, pos - lines->i);
	va_start(ap, message);
	vfprintf(stderr, message, ap);
	if (lines)
		Log(TAG, "%d.%d: %s", num, pos - lines->i, message);


}

void EM_impossible(string msg)
{
	// #Unknown usage
	Log(TAG, "Impossible message: %s", msg);
}

void EM_reset(string msg)
{
	anyErrors = FALSE;
	lineNum = 1;
	linePos = intList(0, NULL);
}
