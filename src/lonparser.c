#include "lon.h"
#include "lonlex.h"
#include "lonparse.h"
#include "lonutil.h"

#include <stdio.h>
#include <string.h>

LONParser *LONParserCreate()
{
	LONParser *p = LON_NEW(LONParser);
	p->lex = LON_NEW(LONLexState);
	p->lex->input = NULL;
	p->parse = LON_NEW(LONParseState);
	p->parse->result = NULL;
	return p;
}

void LONParserDestroy(LONParser *p)
{
	LONLexStateFree(p->lex);
	LONParseStateFree(p->parse);
	free(p);
}

typedef struct chrsta {
	char *base;
	int cur;
	int len;
} chrsta;

int chrread(void *p)
{
	chrsta *s = p;
	if (s->cur < 0) s->cur = 0;
	return s->cur >= s->len ? EOF : s->base[s->cur++];
}

int chrpeek(void *p)
{
	chrsta *s = p;
	if (s->cur < 0) s->cur = 0;
	return s->cur >= s->len ? EOF : s->base[s->cur];
}

long chrtell(void *p)
{
	chrsta *s = p;
	return s->cur;
}

void chrunread(void *p)
{
	chrsta *s = p;
	if (s->cur >= 0) s->cur--;
}

int LONParserParse(LONParser *parser, char *strinput)
{
	LONInput *i; // lexer input source
	chrsta *c; // input state data
	int r = 0; // return code

	c = LON_NEW(chrsta);
	c->base = strinput;
	c->cur = 0;
	c->len = strlen(c->base);

	i = LON_NEW(LONInput);
	i->p = c;
	i->read = &chrread;
	i->peek = &chrpeek;
	i->tell = &chrtell;
	i->unread = &chrunread;

	parser->lex->input = i;
	parser->lex->tokens = NULL;
	parser->lex->tok_alloc = 0;
	parser->lex->tok_count = 0;
	parser->lex->line = 0;

	r = LONLexRun(parser->lex);
	if (r != 0)
		goto cleanup_lexfail;

	parser->parse->tokens = parser->lex->tokens;
	parser->parse->idx = 0;

	r = LONParseRun(parser->parse);
	if (r != 0)
		goto cleanup_parsefail;

cleanup_parsefail:

cleanup_lexfail:
	LON_FREE(i);
	LON_FREE(c);

	return r;
}

/*
 * typedef enum LONTokenType {
	TOK_NONE,
	TOK_EOF,
	TOK_NAME,
	TOK_STR,
	TOK_NUM,
	TOK_LBRACE,
	TOK_RBRACE,
	TOK_LBRACKET,
	TOK_RBRACKET,
	TOK_EQ,
	TOK_SEMICOLON,
	TOK_COMMA,
	TOK_TRUE,
	TOK_FALSE,
	TOK_NULL
} LONTokenType;
*/

// This struct should match the token list in LONTokenType.
static char *token_types[] = {
	"TOK_NONE",
	"TOK_EOF",
	"TOK_NAME",
	"TOK_STR",
	"TOK_NUM",
	"TOK_LBRACE",
	"TOK_RBRACE",
	"TOK_LBRACKET",
	"TOK_RBRACKET",
	"TOK_EQ",
	"TOK_SEMICOLON",
	"TOK_COMMA",
	"TOK_TRUE",
	"TOK_FALSE",
	"TOK_NULL",
	"TOK_COMMENT"
};

void LONDebugDumpTokens(LONParser *p)
{
	// This is a temporary debug function.
	// It prints out each token with the line and type ID.
	LONToken **toks = p->lex->tokens;
	int i = 0;
	for(;;) {
		LONToken *t = toks[i];

		if (t->type == TOK_EOF) {
			break;
		}

		// printf("%s\n", BUF_STR(t->buf));
		printf("token \"%s\" on line %d with type %s / id %d\n",
				BUF_STR(t->buf),
				t->line,
				token_types[t->type],
				t->type);

		i++;
	}
}

void LONDebugFun1(LONParser *p)
{
	// This function is now an alias for LONDebugDumpTokens.
	LONDebugDumpTokens(p);
}

void pval(LONValue *v)
{
	LONTable *tbl;
	LONNode *n;
	switch (v->type) {
	case LON_TYPE_NONE:
		printf("none\n");
		break;
	case LON_TYPE_NUMBER:
		printf("%d\n", v->var.num);
		break;
	case LON_TYPE_STRING:
		printf("%s\n", v->var.str);
		break;
	case LON_TYPE_TABLE:
		tbl = v->var.tbl;
		if (v->var.tbl == NULL || tbl->head == NULL) {
			printf("{ } -- empty table\n");
			return;
		}
		printf("{\n");
		for (n = tbl->head; n != NULL; n = n->next) {
			pval(&n->key);
			pval(&n->val);
		}
		printf("}\n");
		break;
	case LON_TYPE_BOOL:
		v->var.bln ? printf("true\n") : printf("false\n");
		break;
	case LON_TYPE_NIL:
		printf("nil\n");
		break;
	default:
		printf("unrecognised token\n");
		break;
	}
}

void LONDebugDumpSyntaxTree(LONParser *p)
{
	LONTable *t = p->parse->result;
	LONValue *r = LON_NEW(LONValue);
	r->type = LON_TYPE_TABLE;
	r->var.tbl = t;
	pval(r);
}

/* debug: print the whole syntax tree */
void LONDebugFun2(LONParser *p)
{
	// This function is now an alias for LONDebugDumpSyntaxTree.
	LONDebugDumpSyntaxTree(p);
}

LONTable *LONParserParseResult(LONParser *p)
{
	if (p != NULL && p->parse != NULL && p->parse->result != NULL) {
		return p->parse->result;
	} else {
		return NULL;
	}
}
