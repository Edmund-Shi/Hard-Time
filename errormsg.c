#include "errormsg.h"
#include "util.h"
#include "log.h"
#include <stdlib.h>
#include <stdio.h>


bool anyErrors = FALSE;
int EM_tokPos = 0;

static int lineNum = 1;

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

void EM_error(int pos, string message)
{
	IntList lines = linePos;
	int num = lineNum;

	anyErrors = TRUE;
	while (lines && lines->i >= pos){
		lines = lines->rest; 
		num--;
	}
	char msg[256];
	sprintf(msg, "%d.%d: %s", num, pos - lines->i, message);
	logRunningInfo(msg);

}

void EM_impossible(string msg)
{
	// #Unknown usage
	logRunningInfo(msg);
}

void EM_reset(string msg)
{
	anyErrors = FALSE;
	lineNum = 1;
	linePos = intList(0, NULL);
}
