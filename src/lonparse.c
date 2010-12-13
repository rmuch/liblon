#include "lon.h"
#include "lonparse.h"

#include <stdbool.h>

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

static LONValue *ParseLONExpr(LONParseState *s)
{
	LONValue *v = LON_NEW(LONValue);
	LONToken *t;

	if (match(s, TOK_STR)) {
		t = last(s);
		v->type = LON_TYPE_STRING;
		v->var.str = malloc(sizeof(char) * strlen(BUF_STR(t->buf)));
		strcpy(v->var.str, BUF_STR(t->buf));
	} else if (match(s, TOK_NUM)) {
		t = last(s);
		v->type = LON_TYPE_NUMBER;
		v->var.str = malloc(sizeof(char) * 64);
		v->var.num = atoi(BUF_STR(t->buf));
	} else if (match(s, TOK_NAME)) {
		t = last(s);
		v->type = LON_TYPE_STRING;
		v->var.str = malloc(sizeof(char) * strlen(BUF_STR(t->buf)));
		strcpy(v->var.str, BUF_STR(t->buf));
	} else {
		// XXX: error
	}
}

static LONNode *ParseLONField(LONParseState *s)
{

}

static LONTable *ParseLONTable(LONParseState *s)
{
	if (match(s, TOK_LBRACE)) {

	} else {
		// XXX: error
	}
}

static LONTable *ParseLONFile(LONParseState *s)
{
	if (lookAhead(s, TOK_LBRACE)) {
		LONTable *l = ParseLONTable(s);
	} else {
		// XXX: error
	}
}

static int parse(LONParseState *s)
{
	LONTable *h;
	h = ParseLONFile(s);
	return 0;
}

int LONParseRun(LONParseState *s)
{
	int i = parse(s);
	return 0;
}
