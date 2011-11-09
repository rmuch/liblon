#ifndef LONBUFFER_H_
#define LONBUFFER_H_

#include "lonutil.h"

#include <stdlib.h>
#include <string.h>

typedef struct LONBuffer {
	char *data;
	int len;
	int pos;
	int inc;
	int max;
} LONBuffer;

/**
 * Returns a pointer to the data stored by the buffer as a character array.
 */
static inline char *LONBufferToString(LONBuffer *buf) {
	if (buf->data == NULL) {
		return "";
	} else {
		return buf->data;
	}
}

#define BUF_STR(b) LONBufferToString(b)

/**
 * Allocates and initializes a new buffer.
 */
static inline LONBuffer *LONBufferNew(int inc, int max)
{
	LONBuffer *b = LON_NEW(LONBuffer);
	b->data = LON_NEWXC(char, inc);
	b->len = inc;
	b->pos = 0;
	b->inc = inc;
	b->max = max;
	return b;
}

/**
 * Frees memory used by the buffer structure and its data.
 */
static inline void LONBufferFree(LONBuffer *b)
{
	free(b->data);
	free(b);
}

/**
 * Appends a single character to a buffer.
 */
static inline void LONBufferAppend(LONBuffer *b, char c)
{
	if (b->pos == (b->len - 1)) {
		b->data = realloc(b->data, b->len + b->inc);
		memset(b->data + b->len, 0, sizeof(char) * b->inc);
		b->len += b->inc;
	}
	b->data[b->pos++] = c;
}

#define BUF_NEW(inc, max) LONBufferNew(inc, max)
#define BUF_FREE(b) LONBufferFree(b)
#define BUF_APPEND(b, c) LONBufferAppend(b, c)

#endif /* LONBUFFER_H_ */
