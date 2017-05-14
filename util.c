#include "util.h"
#include "log.h"
#include "string.h"

void assert(int errno){
	switch (errno){
		case 0: {
			logRunningInfo("Unknow Error!\n");
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