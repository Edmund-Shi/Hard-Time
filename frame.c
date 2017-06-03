#include "frame.h"
#include "errormsg.h"
#include <stdio.h>
const int F_wordSize=4;

static Temp_temp fp = NULL;
Temp_temp F_FP() {
	if (fp == NULL) {
		fp = Temp_newtemp();
		//printf("FP pointer initialized: %s\n", Temp_look(Temp_name(), fp));
	}
	return fp;
}

static Temp_temp rv = NULL;
Temp_temp F_RV() {
	if (rv == NULL) {
		rv = Temp_newtemp();
		//printf("RV pointer initialized: %s\n", Temp_look(Temp_name(), rv));
	}
	return rv;
}


F_access InFrame(int offset)
{
	F_access tmp = (F_access)checked_malloc(sizeof(*tmp));
	tmp->kind = inFrame;
	tmp->u.offset = offset;
	return tmp;
}

F_access InReg(Temp_temp reg)
{
	F_access tmp = (F_access)checked_malloc(sizeof(*tmp));
	tmp->kind = inReg;
	tmp->u.reg = reg;
	return tmp;
}

F_accessList F_makeaccesslist(F_access head, F_accessList tail) {
	F_accessList temp = (F_accessList)checked_malloc(sizeof(*temp));
	temp->head = head;
	temp->tail = tail;
	return temp;
}

static Temp_temp getNextReg(F_frame frame) {
	frame->locals++;
	return Temp_newtemp();
}

F_frame F_newFrame(Temp_label name, U_boolList formals) {
	F_frame frame = (F_frame)checked_malloc(sizeof(*frame));
	frame->formals = NULL;
	frame->locals = 0;
	frame->offset = 0;
	frame->begin_label = name;
	U_boolList par = formals;
	F_access acc = NULL;
	while (par != NULL) {
		if (par->head == TRUE) {
			acc = InFrame(frame->offset);//#bug offset可能要取绝对值
			frame->offset -= F_wordSize;
		}
		else {
			acc = InReg(getNextReg(frame));
		}
		frame->formals = F_makeaccesslist(acc, frame->formals);
		par = par->tail;
	}

	return frame;
}

Temp_label F_name(F_frame f) {
	return f->begin_label;
	
}

F_accessList F_formals(F_frame f) {
	return f->formals;
}

F_access F_allocLocal(F_frame f, bool escape){
	F_access access;
	if (escape)
	{
		access = InFrame(f->offset);
		f->offset -= F_wordSize;
	}
	else
	{
		access = InReg(Temp_newtemp());
	}
	f->locals = F_makeaccesslist(access, f->locals);
	return access;
}

T_exp F_Exp(F_access acc, T_exp framePtr){
	if (acc->kind == inFrame){
		return T_Mem(T_Binop(T_plus, framePtr, T_Const(acc->u.offset)));
	}
	return  T_Temp(acc->u.reg);
}

T_exp F_externalCall(string s, T_expList args) {
	return T_Call(T_Name(Temp_namedlabel(s)), args);
}

static F_access staticaccess = NULL;
F_access F_staticLink() {
	if (staticaccess == NULL) {
		staticaccess = InFrame(0);
	}
	return staticaccess;
}

