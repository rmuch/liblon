/* wrappers around memory allocation functions */

#ifndef LONUTIL_H_
#define LONUTIL_H_

#include <stdlib.h>

#define LON_NEW(type) (type *) malloc(sizeof(type))
#define LON_NEWC(type) (type *) calloc(1, sizeof(type))
#define LON_NEWXC(type, count) ((type *) calloc(count, sizeof(type)))

#define LON_FREE(i) free(i)

#endif /* LONUTIL_H_ */
