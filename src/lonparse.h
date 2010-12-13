#ifndef LONPARSE_H_
#define LONPARSE_H_

#include "lonlex.h"

typedef struct LONParseState {
	LONToken **tokens;
	int idx;
} LONParseState;

int LONParseRun(LONParseState *s);

#endif /* LONPARSE_H_ */
