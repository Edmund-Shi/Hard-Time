#ifndef LOG_H_
#define LOG_H_
#include "util.h"
void openLog();
void closeLog();
/* Obsolete function!!!*/
void logRunningInfo(char *s);
void Log(string tag, string *message, ...);

void ClearLog();
#endif