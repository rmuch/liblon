#ifndef LONAST_H_
#define LONAST_H_

enum LONASTNodeType
{
	AST_NONE,
	AST_LONDOCUMENT,
	AST_TABLECONSTRUCTOR,
	AST_FIELDLIST,
	AST_FIELD,
	AST_FIELDSEP,
	AST_EXP,
	AST_PREFIXEXP,
};

typedef struct LONASTNode {
	enum LONASTNodeType type;
} LONASTNode;

#endif /* LONAST_H_ */
