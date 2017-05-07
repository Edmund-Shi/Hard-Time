typedef struct A_exp_ *A_exp;
struct A_exp_{
	int kind;
	union {
		int intt;
		struct { A_exp body; } let;
	} u;
};
A_exp A_IntExp(int i);
A_exp A_LetExp(A_exp body);
typedef struct treeNode *Tree;
struct treeNode {
	int kind;
	union {
		A_exp exp;
		int i;
		char *s;
	} u;
};