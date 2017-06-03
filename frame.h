#ifndef FRAME_H_ 
#define FRAME_H_

#include "tree.h"
#include "temp.h"

typedef struct F_access_ *F_access;
struct F_access_ {
	enum { inFrame, inReg } kind;
	union {
		int offset;
		Temp_temp reg;
	} u;
};

typedef struct F_accessList_ *F_accessList;
struct F_accessList_ {
	F_access head;
	F_accessList tail;
};

typedef struct F_frame_ *F_frame;
struct F_frame_ {
	F_accessList formals; //All of the vars stored in the frame
	int locals;
	int offset;
	Temp_label begin_label;
};

// function decs in book
F_frame F_newFrame(Temp_label name, U_boolList formals);
Temp_label F_name(F_frame f);
F_accessList F_formals(F_frame f);
F_access F_allocLocal(F_frame f, bool escape) ;
F_access F_staticLink();

//Registers

Temp_temp F_FP(void);//frame pointer
Temp_temp F_RV(void);//return value, a shortcut to F_VN

extern const int F_wordSize;
T_exp F_Exp(F_access acc, T_exp framePtr);
T_exp F_externalCall(string s, T_expList args);
#endif // ifndef FRAME_H_
