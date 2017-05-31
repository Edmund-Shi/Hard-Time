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

//Stack Structure Implement Begin

//Push
void GS_push(stack_node *plist, void *key) {
	int size = sizeof(struct stack_node_);
	stack_node list = *plist;
	if (list == NULL) {
		list = checked_malloc(size);
		list->key = key;
		list->last = NULL;
	}
	else {
		stack_node head = checked_malloc(size);
		head->key = key;
		head->last = list;
		list = head;
	}
	*plist = list;
}
//Pop
void GS_pop(stack_node *plist) {
	stack_node list = *plist;
	if (list != NULL) {
		stack_node head = list;
		list = head->last;
		free(head);
	}
	*plist = list;
}
//Peek(Top)
void *GS_peek(stack_node *plist) {
	return (*plist)->key;
}
//empty
void GS_empty(stack_node *plist) {
	while (*plist != NULL)
		GS_pop(plist);
}
//check
bool GS_check(stack_node list, void* key, bool(*compare)(void*, void*)) {
	stack_node cursor;
	for (cursor = list; cursor != NULL; cursor = cursor->last)
		if (compare(cursor->key, key))return TRUE;
	return FALSE;
}