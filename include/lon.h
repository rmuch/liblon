#ifndef LON_H_
#define LON_H_

#define LON_API

#include <stdbool.h>

struct LONTable;

typedef union LONVar {
	char *str;
	int num;
	struct LONTable *tbl;
	bool bln;
} LONVar;

typedef enum LONType {
	LON_TYPE_NONE,
	LON_TYPE_NIL,
	LON_TYPE_STRING,
	LON_TYPE_NUMBER,
	LON_TYPE_BOOL,
	LON_TYPE_TABLE
} LONType;

typedef struct LONValue {
	LONType type;
	LONVar var;
} LONValue;

typedef struct LONNode {
	LONValue key;
	LONValue val;
	struct LONNode *next;
} LONNode;

typedef struct LONTable {
	LONNode *head;
} LONTable;

struct LONLexState;
struct LONParseState;

typedef struct LONParser {
	struct LONLexState *lex;
	struct LONParseState *parse;
} LONParser;

LON_API void LONDebugFun1(LONParser *p);
LON_API void LONDebugFun2(LONParser *p);

LON_API LONParser *LONParserCreate();
LON_API void LONParserDestroy(LONParser *parser);

LON_API int LONParserParse(LONParser *parser, char *strinput);
LON_API int LONParserParseFile(LONParser *parser, char *path);

#endif /* LON_H_ */
