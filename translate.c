#include "translate.h"
#include "tree.h"

//functions to translate tree into tr_exp
//only available in c file
static Tr_exp Tr_Ex(T_exp ex);
static Tr_exp Tr_Nx(T_stm nx);
static Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm);

static T_exp unEx(Tr_exp e);
static T_stm unNx(Tr_exp e);
static struct Cx unCx(Tr_exp e);

static Tr_level outermostLevel = NULL;
Tr_level Tr_outermost() {
	if (outermostLevel == NULL){
		outermostLevel = (Tr_level)checked_malloc(sizeof(struct Tr_level_));
		outermostLevel->parent = NULL;
		outermostLevel->depth = 0;
		// 最外层的一层并不是真正的main，而是包含有预处理信息的
		outermostLevel->frame = F_newFrame(Temp_namedlabel("_main"), NULL);
	}
	return outermostLevel;
}