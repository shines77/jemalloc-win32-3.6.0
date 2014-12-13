#ifndef JEMALLOC_STDBOOL_H
#define JEMALLOC_STDBOOL_H

#include <wtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/* MSVC doesn't define _Bool or bool in C, but does have BOOL */
/* Note this doesn't pass autoconf's test because (bool) 0.5 != true */
typedef BOOL    _Bool;

#define bool    _Bool
#define true    1
#define false   0

#define __bool_true_false_are_defined 1

#ifdef __cplusplus
}
#endif

#endif  /* JEMALLOC_STDBOOL_H */
