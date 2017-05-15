#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "log.h"

static FILE *gfp = NULL; /* global log file pointer */

/* Obsolete function!!!*/
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

/*Clear log file at the beginning of the program*/
void ClearLog(void) {
	if (gfp != NULL){
		fclose(gfp);
	}
	/* Open log file*/
	if ((gfp = fopen("log.txt", "w")) == NULL) {
		printf("open log file failed!\n");
		return;
	}
	fclose(gfp);
	return;
}

void Log(string tag, string *message, ...) {
	va_list ap;
	va_start(ap, message);
	fprintf(gfp, "%s: ", tag);
	vfprintf(gfp, message, ap);
	fprintf(gfp, "\n");
	fflush(gfp);
	va_end(ap);
}
void openLog() {
	if ((gfp = fopen("log.txt", "a+")) == NULL) {
		printf("open log file failed!\n");
		exit(0);
	}
}
void closeLog() {
	fclose(gfp);
}