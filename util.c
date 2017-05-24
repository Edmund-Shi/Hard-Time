#include "util.h"
#include "log.h"
#include "string.h"

void assert(int errno){
	switch (errno){
		case 0: {
			Log("Assert 0", "Unknow Error!\nExit program!");
			exit(0);
			return;
		}
	}
}
string String(char *s) {
	string p = checked_malloc(strlen(s) + 1);
	strcpy(p, s);
	return p;
}

U_boolList U_BoolList(bool head, U_boolList tail) {
	U_boolList list = checked_malloc(sizeof(*list));
	list->head = head;
	list->tail = tail;
	return list;
}