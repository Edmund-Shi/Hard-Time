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

static Temp_temp ra = NULL;
Temp_temp F_RA() {
	if (ra == NULL) {
		ra = Temp_newtemp();
		//printf("RA pointer initialized: %s\n", Temp_look(Temp_name(), ra));
	}
	return ra;
}

static Temp_temp rv = NULL;
Temp_temp F_RV() {
	if (rv == NULL) {
		rv = F_VN(0);
		//printf("RV pointer initialized: %s\n", Temp_look(Temp_name(), rv));
	}
	return rv;
}

static Temp_temp sp = NULL;
Temp_temp F_SP() {
	if (sp == NULL) {
		sp = Temp_newtemp();
		//printf("SP pointer initialized: %s\n", Temp_look(Temp_name(), sp));
	}
	return sp;
}

static Temp_temp zero = NULL;
Temp_temp F_ZERO() {
	if (zero == NULL) {
		zero = Temp_newtemp();
		//printf("ZERO pointer initialized: %s\n", Temp_look(Temp_name(), zero));
	}
	return zero;
}

static Temp_temp sn[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
static Temp_temp tn[10] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
static Temp_temp an[4] = { NULL, NULL, NULL, NULL };
static Temp_temp vn[2] = { NULL, NULL };

Temp_temp F_SN(int i) {
	if (i >= 0 && i <= 7) {
		if (sn[i] == NULL) {
			sn[i] = Temp_newtemp();
		}
		return sn[i];
	}
	else {
		EM_error(i, "Internal error: using a pointer that is not available for MIPS.");
		//printf("Internal error: using a pointer that is not available for MIPS.");
		exit(2);
	}
}

Temp_temp F_TN(int i) {
	if (i >= 0 && i <= 9) {
		if (tn[i] == NULL) {
			tn[i] = Temp_newtemp();
		}
		return tn[i];
	}
	else {
		printf("Internal error: using a pointer that is not available for MIPS.");
		exit(2);
	}
}

Temp_temp F_AN(int i) {
	if (i >= 0 && i <= 3) {
		if (an[i] == NULL) {
			an[i] = Temp_newtemp();
		}
		return an[i];
	}
	else {
		printf("Internal error: using a pointer that is not available for MIPS.");
		exit(2);
	}
}

Temp_temp F_VN(int i) {
	if (i >= 0 && i <= 1) {
		if (vn[i] == NULL) {
			vn[i] = Temp_newtemp();
		}
		return vn[i];
	}
	else {
		printf("Internal error: using a pointer that is not available for MIPS.");
		exit(2);
	}
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
		if (par->head != NULL) {
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

F_frag F_StringFrag(Temp_label label, string str) {
	F_frag temp = checked_malloc(sizeof(struct F_frag_));
	temp->kind = F_stringFrag;
	temp->u.stringg.label = label;
	temp->u.stringg.str = str;
	return temp;
}

F_frag F_ProcFrag(T_stm body, F_frame frame) {
	F_frag temp = checked_malloc(sizeof(struct F_frag_));
	temp->kind = F_procFrag;
	temp->u.proc.body = body;
	temp->u.proc.frame = frame;
	return temp;
}



F_fragList F_FragList(F_frag head, F_fragList tail) {
	F_fragList temp = (F_fragList)checked_malloc(sizeof(struct F_fragList_));
	temp->head = head;
	temp->tail = tail;
	return temp;
}

//?? 下面有问题
/*

}
*/
static F_fragList *fragList = NULL;
static F_fragList fragList_head = NULL;
static F_frag* extendFragList() {
	if (fragList == NULL) {
		fragList = (F_fragList*)checked_malloc(sizeof(struct F_fragList_));
	}

	*fragList = (F_fragList)checked_malloc(sizeof(struct F_fragList_));

	if (fragList_head == NULL) {
		//Remember the head of frag list
		fragList_head = *fragList;
	}
	F_frag *currentFrag = &((*fragList)->head);
	fragList = &((*fragList)->tail);
	*fragList = NULL;

	return currentFrag;
}
void F_String(Temp_label label, string str) {
	F_frag *currentFrag = extendFragList();
	*currentFrag = F_StringFrag(label, str);
}
void F_Proc(T_stm body, F_frame frame) {
	F_frag *currentFrag = extendFragList();
	*currentFrag = F_ProcFrag(body, frame);
	//printf("New proc added to frag list\n");fflush(stdout);
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

static Temp_tempList registerLists[4] = { NULL, NULL, NULL, NULL };
Temp_tempList F_getRegList(RL_type type) {
	int tp = (int)type;
	Temp_tempList temp;
	int i;//#bug 可能有
	if (registerLists[tp] == NULL) {
		switch (type) {
		case specialregs:
			temp= Temp_TempList(F_RV(), Temp_TempList(F_RA(), Temp_TempList(F_FP(), Temp_TempList(F_SP(), NULL))));
			registerLists[tp] = temp;
			break;
		case argregs:
			for (i = 3; i >= 0; i++) {
				if (i == 3)
					temp = Temp_TempList(F_AN(3), NULL);
				else
					temp = Temp_TempList(F_AN(0), temp);
			}
			registerLists[tp] = temp;
			break;
		case calleesaves:
			for (i = 7; i >= 0; i++) {
				if(i == 7)
					temp= Temp_TempList(F_SN(7), NULL);
				else
					temp = Temp_TempList(F_SN(i), temp);
			}
			registerLists[tp] = temp;
			break;
		case callersaves:
			for (i = 9; i >= 0; i++) {
				if (i == 9)
					temp = Temp_TempList(F_TN(9), NULL);
				else
					temp = Temp_TempList(F_TN(i), temp);
			}
			registerLists[tp] = temp;
			break;
		}
	}
	return registerLists[tp];
}

static F_access staticaccess = NULL;
F_access F_staticLink() {
	if (staticaccess == NULL) {
		staticaccess = InFrame(0);
	}
	return staticaccess;
}

