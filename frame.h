#ifndef FRAME_H_ 
#define FRAME_H_

#include "tree.h"
#include "temp.h"

// frame and access decs
typedef struct F_frame_ *F_frame;
struct F_frame_ {
	F_accessList formals; //All of the vars stored in the frame
	int locals;
	int offset;
	Temp_label begin_label; 
};
typedef struct F_access_ *F_access;
struct F_access_ {
	enum  	{ inFrame,inReg	} kind;
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

typedef struct F_frag_ *F_frag;
struct F_frag_ {
	enum {F_stringFrag,F_procFrag} kind;
	union {
		struct {
			Temp_label label;
			string str;
		}stringg;
		struct {
			T_stm body;
			F_frame frame;
		} proc;
	}u ;
};
F_frag F_StringFrag(Temp_label label, string str);
F_frag F_ProcFrag(T_stm body, F_frame frame);

void F_String(Temp_label label, F_frame frame);
void F_Proc(T_stm body, F_frame frame);

typedef struct F_fragList_ *F_fragList;
struct F_fragList_ {
	F_frag head;
	F_fragList tail;
};

F_fragList F_FragList(F_frag head, F_fragList tail);
F_fragList F_getFragList(void);

// function decs in book
F_frame F_newFrame(Temp_label name, U_boolList formals);
Temp_label F_name(F_frame f);
F_accessList F_formals(F_frame f);
F_access F_allocLocal(F_frame f, bool escape);
F_access F_staticLink();

//Registers

Temp_temp F_FP(void);//frame pointer
Temp_temp F_RA(void);//return address
Temp_temp F_RV(void);//return value, a shortcut to F_VN
Temp_temp F_SP(void);//stack pointer
Temp_temp F_ZERO(void);//constant zero
Temp_temp F_SN(int i);//generic, saved by callee
Temp_temp F_TN(int i);//generic, saved by caller
Temp_temp F_AN(int i);//argument
Temp_temp F_VN(int i);//return values

//The mapping of special registers and their names
Temp_map F_tempMap(void);

typedef enum {
	specialregs = 0, 	//special regs like FP, RV, RA
	argregs, 	//regs to pass arguments
	calleesaves, 	//regs that callee must not change (or save-restore)
	callersaves	//regs that caller must preserve since callee may affect them
} RL_type;
//Get one of four set of regs on MIPS CPU
Temp_tempList F_getRegList(RL_type type);

extern const int F_wordSize;
T_exp F_Exp(F_access acc, T_exp framePtr);
T_exp F_externalCall(string s, T_expList args);
#endif // ifndef FRAME_H_
