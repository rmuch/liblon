#ifndef LON_H_
#define LON_H_

#define LON_API

#include <stdbool.h>

struct LONTable;

/**
 * Union type representing any LON value.
 */
typedef union LONVar {
	char *str;
	int num;
	struct LONTable *tbl;
	bool bln;
} LONVar;

/**
 * Enumeration to represent any possible LON value type.
 */
typedef enum LONType {
	LON_TYPE_NONE,
	LON_TYPE_NIL,
	LON_TYPE_STRING,
	LON_TYPE_NUMBER,
	LON_TYPE_BOOL,
	LON_TYPE_TABLE
} LONType;

/**
 * A LON value paired with its value type.
 */
typedef struct LONValue {
	LONType type;
	LONVar var;
} LONValue;

/**
 * Linked-list table type, with a key value pair and a single forward link to
 * the next element.
 */
typedef struct LONNode {
	LONValue key;
	LONValue val;
	struct LONNode *next;
} LONNode;

/**
 * Structure to store the head linked list node of a table.
 */
typedef struct LONTable {
	LONNode *head;
} LONTable;

/**
 * Internal state of liblon's lexer.
 */
struct LONLexState;

/**
 * Internal state of liblon's parser.
 */
struct LONParseState;

/**
 * An object storing liblon's internal parser state.
 */
typedef struct LONParser {
	struct LONLexState *lex;
	struct LONParseState *parse;
} LONParser;

/**
 * Deprecated alias for LONDebugDumpTokens.
 *
 * @deprecated See LONDebugDumpTokens.
 */
LON_API void LONDebugFun1(LONParser *p);

/**
 * Deprecated alias for LONDebugDumpSyntaxTree.
 *
 * @deprecated See LONDebugDumpSyntaxTree.
 */
LON_API void LONDebugFun2(LONParser *p);

/**
 * Debug function to dump token list to stdout.
 */
LON_API void LONDebugDumpTokens(LONParser *parser);

/**
 * Debug function to dump syntax tree to stdout.
 */
LON_API void LONDebugDumpSyntaxTree(LONParser *parser);

/**
 * Allocates and initializes a new LON parser.
 *
 * @returns A new LON parser instance.
 */
LON_API LONParser *LONParserCreate();

/**
 * Frees memory allocated by a LON parser.
 *
 * @param parser An instantiated parser state object.
 */
LON_API void LONParserDestroy(LONParser *parser);

/**
 * Parses a LON object from a string.
 *
 * @param parser   An instantiated parser state object.
 * @param strinput A null-terminated string to be treated as raw input.
 *
 * @returns Status code of 0 upon success, or a non-zero value on failure.
 */
LON_API int LONParserParse(LONParser *parser, char *strinput);

/**
 * Parses a LON object from a file path.
 */
LON_API int LONParserParseFile(LONParser *parser, char *path);

/**
 * Retrieves the parser result.
 *
 * @returns Parsed object table, or NULL on failure.
 */
LON_API LONTable *LONParserParseResult(LONParser *parser);

#endif /* LON_H_ */
