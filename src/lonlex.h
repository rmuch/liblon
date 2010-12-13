#ifndef LONLEX_H_
#define LONLEX_H_

#include "lonbuffer.h"
#include "lonutil.h"

#include <stdlib.h>

typedef struct LONInput {
	void *p;
	int (*read)(void *);
	int (*peek)(void *);
	long (*tell)(void *);
	void (*unread)(void *);
} LONInput;

static inline int LONInputRead(LONInput *i) { return i->read(i->p); }
static inline int LONInputPeek(LONInput *i) { return i->peek(i->p); }
static inline long LONInputTell(LONInput *i) { return i->tell(i->p); }
static inline void LONInputUnread(LONInput *i) { return i->unread(i->p); }

typedef enum LONTokenType {
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

typedef struct LONToken {
	LONTokenType type;
	LONBuffer *buf;
	int line;
} LONToken;

void LONTokenListFree(LONToken **tl);

static inline LONToken *LONTokenNew() {
	LONToken *tok = LON_NEW(LONToken);
	tok->type = TOK_NONE;
	tok->buf = BUF_NEW(10, -1);
	tok->line = 0;
	return tok;
}

static inline void LONTokenFree(LONToken *t) {
	BUF_FREE(t->buf);
	free(t);
	return;
}

#define TOK_NEW() LONTokenNew()
#define TOK_FREE() LONTokenFree()

typedef enum LONLexStateType {
	LEX_DEFAULT,
	LEX_NAME,
	LEX_STRING,
	LEX_NUMBER,
} LONLexStateType;

typedef struct LONLexState {
	LONInput *input;
	LONLexStateType type;
	LONToken **tokens;
	int tok_alloc;
	int tok_count;
	int line;
} LONLexState;

int LONLexRun(LONLexState *state);
void LONLexStateAppendToken(LONLexState *sta, LONToken *tok);

void LONLexStateFree(LONLexState *state);

#define TOK_APPEND(sta, tok) LONLexStateAppendToken(sta, tok)

#endif /* LONLEX_H_ */
