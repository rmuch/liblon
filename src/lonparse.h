#ifndef LONPARSE_H_
#define LONPARSE_H_

#include "lonlex.h"

typedef struct LONParseState {
	LONToken **tokens;
	int idx;
	LONTable *result;
} LONParseState;

int LONParseRun(LONParseState *s);
void LONParseStateFree(LONParseState *state);

#endif /* LONPARSE_H_ */
