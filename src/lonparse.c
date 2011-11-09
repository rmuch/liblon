#include "lon.h"
#include "lonparse.h"

#include <stdio.h>
#include <stdbool.h>

/*
 * TODO: Improve the parser, error reporting.
 * If there is a parse error, the whole thing breaks.
 */

static bool lookAhead(LONParseState *s, LONTokenType t)
{
	if (s->tokens[s->idx]->type == t) {
		return true;
	} else {
		return false;
	}
}

static bool consume(LONParseState *s, LONTokenType t)
{
	if (s->tokens[s->idx]->type == t) {
		s->idx++;
		return true;
	} else {
		return false;
	}
}

static bool match(LONParseState *s, LONTokenType t)
{
	if (s->tokens[s->idx]->type == t) {
		s->idx++;
		return true;
	} else {
		return false;
	}
}

static LONToken *last(LONParseState *s)
{
	if (s->idx > 0) {
		return s->tokens[s->idx - 1];
	} else {
		return s->tokens[s->idx];
	}
}

#define FILL(xval, xtype, xvar) {\
	v->type = xtype;

// Forward declaration.
static LONTable *ParseLONTable(LONParseState *s);

static LONValue *ParseLONExpr(LONParseState *s)
{
	LONValue *v = NULL;
	LONToken *t;
	LONTable *b;
	bool c;

	if (consume(s, TOK_STR)) {
		t = last(s);
		v = LON_NEW(LONValue);
		v->type = LON_TYPE_STRING;
		v->var.str = malloc(sizeof(char) * strlen(BUF_STR(t->buf)));
		strcpy(v->var.str, BUF_STR(t->buf));
	} else if (consume(s, TOK_NUM)) {
		t = last(s);
		v = LON_NEW(LONValue);
		v->type = LON_TYPE_NUMBER;
		// v->var.str = malloc(sizeof(char) * 64);
		v->var.num = atoi(BUF_STR(t->buf));
	} /* else if (match(s, TOK_NAME)) {
		t = last(s);
		v->type = LON_TYPE_STRING;
		v->var.str = malloc(sizeof(char) * strlen(BUF_STR(t->buf)));
		strcpy(v->var.str, BUF_STR(t->buf));
	} */ else if (lookAhead(s, TOK_LBRACE)) {
		b = ParseLONTable(s);
		if (b == NULL) {
			fprintf(stderr, "Malformed table in expression.\n");
			return NULL;
		}
		v = LON_NEW(LONValue);
		v->type = LON_TYPE_TABLE;
		v->var.tbl = b;
	} else if (lookAhead(s, TOK_TRUE) || lookAhead(s, TOK_FALSE)) {
		v = LON_NEW(LONValue);
		v->type = LON_TYPE_BOOL;
		c = consume(s, TOK_TRUE);
		if (c == false) {
			c = consume(s, TOK_FALSE);
			v->var.bln = false;
		} else {
			v->var.bln = true;
		}
	} else if (consume(s, TOK_NULL)) {
		v = LON_NEW(LONValue);
		v->type = LON_TYPE_NIL;
		v->var.num = 0;
	} else {
		fprintf(stderr, "Expected TOK_STR or TOK_NUM in expression.\n");
	}

	return v;
}

static LONNode *ParseLONField(LONParseState *s)
{
	LONNode *nod = LON_NEW(LONNode);
	LONToken *t;
	LONValue *vala, *valb;
	nod->next = NULL;
	// <Name> '=' <expr>
	if (consume(s, TOK_NAME)) {
		t = last(s);
		vala = LON_NEW(LONValue);
		vala->type = LON_TYPE_STRING;
		vala->var.str = strdup(BUF_STR(t->buf));
		if (!consume(s, TOK_EQ)) {
			fprintf(stderr, "Expected TOK_EQ after TOK_NAME in field list on line %d.\n", t->line);
			goto cleanup;
		}
		valb = ParseLONExpr(s);
		if (valb == NULL) {
			fprintf(stderr, "Expected expression after TOK_EQ.\n");
			goto cleanup;
		}
		nod->key = *vala;
		nod->next = NULL;
		return nod;
	}
	// '[' <expr> ']' = <expr>
	else if (consume(s, TOK_LBRACKET)) {
		vala = ParseLONExpr(s);
		if (!consume(s, TOK_RBRACKET) || !consume(s, TOK_EQ)) {
			fprintf(stderr, "Expected TOK_RBRACKET TOK_EQ after expression in field list.\n");
			return NULL;
		}
		valb = ParseLONExpr(s);/*
		if (!consume(s, TOK_RBRACKET)) {
			fprintf(stderr, "Expected closing TOK_RBRACKET after expression.\n");
			return NULL;
		}*/
		nod->key = *vala;
		nod->val = *valb;
		nod->next = NULL;
		return nod;
	}
	// <expr>
	else {
		vala = ParseLONExpr(s);
		nod->key.type = LON_TYPE_NONE;

		// XXX: We have a crash here when there are no items in a table.
		//      Temporary workaround? I think this should do it.

		if (vala != NULL) {
			nod->val = *vala;
		} else {
			return NULL;
		}

		nod->next = NULL;
		return nod;
	}
	/*
	else if (lookAhead(s, TOK_STR) ||
			lookAhead(s, TOK_NUM)) {
		vala = ParseLONExpr(s);
	}*/

cleanup:
	LON_FREE(nod);
	LON_FREE(vala);
	LON_FREE(valb);
	return NULL;
}

static LONTable *ParseLONTable(LONParseState *s)
{
	LONTable *tbl = LON_NEW(LONTable);
	LONNode *b, *c, *t; // base, current, temp

	// '{' <fields> '}'
	if (consume(s, TOK_LBRACE)) {
		// first field
		b = ParseLONField(s);
		if (b == NULL) {
			if (!consume(s, TOK_RBRACE)) {
				fprintf(stderr, "Expected field.\n");
				goto cleanup;
			} else {
				tbl->head = NULL;
				goto success;
			}
		}
		// keep going, appending to linked list
		while (consume(s, TOK_COMMA) || consume(s, TOK_SEMICOLON)) {
			// TODO: The algorithmic complexity of this is bad, as we're looping over the list each time.
			// Fix it soon.
			c = ParseLONField(s);
			if (c != NULL) {
				for(t = b; t->next != NULL; t = t->next);
				t->next = c;
			} else {
				fprintf(stderr, "Expected field.\n");
				goto cleanup;
			}
		}
		tbl->head = b;
		if (!consume(s, TOK_RBRACE)) {
			fprintf(stderr, "Expected TOK_RBRACE.\n");
			goto cleanup;
		}
	} else {
		fprintf(stderr, "Expected TOK_LBRACE.\n");
		goto cleanup;
	}
success:
	return tbl;
cleanup:
	LON_FREE(tbl);
	return NULL;
}

static LONTable *ParseLONFile(LONParseState *s)
{
	if (lookAhead(s, TOK_LBRACE)) {
		LONTable *l = ParseLONTable(s);
		return l;
	} else {
		fprintf(stderr, "Expected TOK_LBRACE.\n");
		return NULL;
	}
}

static int parse(LONParseState *s)
{
	LONTable *h;
	h = ParseLONFile(s);
	s->result = h;
	return 0;
}

int LONParseRun(LONParseState *s)
{
	int i = parse(s);
	return 0;
}

void LONParseStateFree(LONParseState *state)
{
	// TODO: Clean up token list (do we want to do this?)
	free(state);
}
