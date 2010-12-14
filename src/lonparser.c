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

void LONDebugFun1(LONParser *p)
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
		printf("token \"%s\" on line %d with typeid %d\n",
				BUF_STR(t->buf),
				t->line,
				t->type);

		i++;
	}
}

void pval(LONValue *v)
{

}

void LONDebugFun2(LONParser *p)
{

}
