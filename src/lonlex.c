#include "lon.h"
#include "lonlex.h"

#include <stdio.h>
#include <ctype.h>

/*
static LONToken *lex(LONLexState *state)
{
	int c;
	LONToken *t = TOK_NEW();
	state->type = LEX_DEFAULT;

	for(;;) {
		if (t->type != TOK_NONE) {
			// LONInputUnread(state->input);
			return t;
		}
		c = LONInputRead(state->input);
		if (c == '\n')
			state->line++;

		if (c == EOF) {
			t->type = TOK_EOF;
			return t;
		}

		switch (state->type) {
		case LEX_DEFAULT:
			if (isalpha(c)) {
				state->type = LEX_NAME;
				LONInputUnread(state->input);
			} else if (isdigit(c)) {
				state->type = LEX_NUMBER;
				LONInputUnread(state->input);
			} else if (c == '"') {
				state->type = LEX_STRING;
				LONInputUnread(state->input);
			} else if (c == '{') {
				t->type = TOK_LBRACE;
				t->line = state->line;
				BUF_APPEND(t->buf, c);
			} else if (c == '}') {
				t->type = TOK_RBRACE;
				t->line = state->line;
				BUF_APPEND(t->buf, c);
			} else if (c == '[') {
				t->type = TOK_LBRACKET;
				t->line = state->line;
				BUF_APPEND(t->buf, c);
			} else if (c == ']') {
				t->type = TOK_RBRACKET;
				t->line = state->line;
				BUF_APPEND(t->buf, c);
			} else if (c == '=') {
				t->type = TOK_EQ;
				t->line = state->line;
				BUF_APPEND(t->buf, c);
			} else if (c == ';') {
				t->type = TOK_SEMICOLON;
				t->line = state->line;
				BUF_APPEND(t->buf, c);
			} else if (c == ',') {
				t->type = TOK_COMMA;
				t->line = state->line;
				BUF_APPEND(t->buf, c);
			}
			break;
		case LEX_NAME:
			if (isalpha(c)) {
				BUF_APPEND(t->buf, c);
			} else {
				t->type = TOK_NAME;
				LONInputUnread(state->input);
			}
			break;
		case LEX_STRING:
			if (c != '"') {
				BUF_APPEND(t->buf, c);
			} else {
				BUF_APPEND(t->buf, c);
				t->type = TOK_NAME;
				LONInputUnread(state->input);
			}
			break;
		case LEX_NUMBER:
			if (isdigit(c)) {
				BUF_APPEND(t->buf, c);
			} else {
				t->type = TOK_NUM;
				LONInputUnread(state->input);
			}
			break;
		}
	}
}
*/

#define LEX_EAT_ONE() \
		BUF_APPEND(t->buf, c); \
		LONInputRead(state->input);

#define LEX_SINGLE(x, y) \
	else if (c == x) { \
		t->type = y; \
		LEX_EAT_ONE(); \
	}


static LONToken *lex(LONLexState *state)
{
	LONToken *t = TOK_NEW();
	int c;

	state->type = LEX_DEFAULT;

	for (;;) {
		if (t->type != TOK_NONE) {
			t->line = state->line;
			t->col = state->col; // TODO: Fix this. It's broken.
			return t;
		}

		// XXX: Token position may not be saved properly.
		c = LONInputPeek(state->input);

		if (c == '\n') {
			state->line++;
			LONInputRead(state->input); // TODO: improve
		}

		if (c == EOF) {
			t->type = TOK_EOF;
			return t;
		}

		// Macro soup!
		switch (state->type) {
		case LEX_DEFAULT:
			if (isalpha(c)) {
				state->type = LEX_NAME;
			} else if (isdigit(c)) {
				state->type = LEX_NUMBER;
			} else if (c == '"') {
				state->type = LEX_STRING;
				LEX_EAT_ONE()
			} else if (c == '-') {
				if (LONInputPeek(state->input) == '-') {
					state->type = LEX_SLCOMMENT;
				}
			}
			LEX_SINGLE('{', TOK_LBRACE)
			LEX_SINGLE('}', TOK_RBRACE)
			LEX_SINGLE('[', TOK_LBRACKET)
			LEX_SINGLE(']', TOK_RBRACKET)
			LEX_SINGLE(',', TOK_COMMA)
			LEX_SINGLE(';', TOK_SEMICOLON)
			LEX_SINGLE('=', TOK_EQ)
			else {
				LONInputRead(state->input);
			}
			break;
		case LEX_NAME:
			if (isalpha(c)) {
				LEX_EAT_ONE()
			} else {
				if (strcmp(BUF_STR(t->buf), "true") == 0) {
					t->type = TOK_TRUE;
				} else if (strcmp(BUF_STR(t->buf), "false") == 0) {
					t->type = TOK_FALSE;
				} else if (strcmp(BUF_STR(t->buf), "nil") == 0) {
					t->type = TOK_NULL;
				} else {
					t->type = TOK_NAME;
				}
				state->type = LEX_DEFAULT;
			}
			break;
		case LEX_STRING:
			if (c != '"') {
				LEX_EAT_ONE()
			} else {
				LEX_EAT_ONE()
				t->type = TOK_STR;
				state->type = LEX_DEFAULT;
			}
			break;
		case LEX_NUMBER:
			if (isdigit(c)) {
				LEX_EAT_ONE()
			} else {
				t->type = TOK_NUM;
				state->type = LEX_DEFAULT;
			}
			break;
		case LEX_SLCOMMENT:
			if (c == '\n') {
				// TODO: discard
				state->type = LEX_DEFAULT;
			} else {
				LEX_EAT_ONE()
			}
			break;
		}
	}
}

int LONLexRun(LONLexState *state)
{
	LONToken *next;
	while ((next = lex(state)) != NULL) {
		TOK_APPEND(state, next);
		if (next->type == TOK_EOF)
			goto success;
	}
	if (next == NULL) return -1; // TODO: free
	success: return 0;
}

#define LEX_STATE_REALLOC_GROWTH 20

void LONLexStateAppendToken(LONLexState *sta, LONToken *tok) {
	// TODO: Move reallocation growth size to a constant or variable.
	if (sta->tok_count == sta->tok_alloc) {
		if (sta->tokens == NULL) {
			sta->tokens = malloc(sizeof(LONToken *) * LEX_STATE_REALLOC_GROWTH);
			sta->tok_alloc += LEX_STATE_REALLOC_GROWTH;
		} else {
			sta->tokens = realloc(sta->tokens, sizeof(LONToken *) * (sta->tok_alloc + LEX_STATE_REALLOC_GROWTH));
			sta->tok_alloc += LEX_STATE_REALLOC_GROWTH;
		}
	}
	sta->tokens[sta->tok_count++] = tok;
}

void LONLexStateFree(LONLexState *state)
{
	// TODO: Clean up token list (do we want to do this?)
	free(state);
}

void LONTokenListFree(LONToken **tl)
{
	int i;
	LONToken *t;
	for (i = 0; ; i++) {
		t = tl[i];
		if (t->type == TOK_EOF) {
			LONTokenFree(t);
			break;
		}
		LONTokenFree(t);
	}
}
