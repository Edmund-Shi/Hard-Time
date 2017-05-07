#include <stdio.h>
#include <stdlib.h>
#include "log.h"

void logRunningInfo(char *s){
	FILE *fp;
	if((fp = fopen("log.txt", "a+")) == NULL){
		printf("open log file failed!\n");
		return ;
	}
	fprintf(fp, "%s\n", s);
	fflush(fp);
	fclose(fp);
	return ;
}