//
// jemalloc_test.cpp
//

#include "jemalloc_test.h"

#include "jemalloc/jemalloc.h"

#include "mempool_tester.h"
#include "malloc_tester.h"
#include "jemalloc_tester.h"

#include "lang_def.h"

#include <stdlib.h>
#include <stdio.h>

#include <assert.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <string.h>
#include <conio.h>
#include <errno.h>

#define FIXED_RANDOM_SEED       31415926UL
#define MAX_LOOP_COUNT          1000000

test_data_t orig_malloc_test_params[] = {
    // define fixed size and repeated alloca
    ST_FIXED_REPEATED,  ALLOC_WAYS_NONE,        0,          0,          0,          0,      0,

    // fixed size and repeated alloca
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      8,          8,          1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      16,         16,         1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      32,         32,         1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      64,         64,         1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      128,        128,        1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      256,        256,        1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      512,        512,        1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      1024,       1024,       1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      2048,       2048,       1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      4096,       4096,       1000000,    0,      0,

    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 13),  (1 << 13),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 14),  (1 << 14),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 15),  (1 << 15),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 16),  (1 << 16),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 17),  (1 << 17),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 18),  (1 << 18),  1000000,    0,      0,

#ifndef _DEBUG
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 19),  (1 << 19),  100000,     0,      0,

    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 20),  (1 << 20),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 21),  (1 << 21),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 22),  (1 << 22),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 23),  (1 << 23),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 24),  (1 << 24),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 25),  (1 << 25),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 26),  (1 << 26),  100000,     0,      0,
#else
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 19),  (1 << 19),  100000,     0,      0,

    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 20),  (1 << 20),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 21),  (1 << 21),  50000,      0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 22),  (1 << 22),  50000,      0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 23),  (1 << 23),  25000,      0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 24),  (1 << 24),  10000,      0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 25),  (1 << 25),  10000,      0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 26),  (1 << 26),  10000,      0,      0,
#endif

    // define continuous size and repeated alloca
    ST_CONTI_REPEATED,  ALLOC_WAYS_NONE,        0,          0,          0,          0,      0,

    // continuous size and repeated alloca
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          256,        1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          512,        1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          1024,       1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          2048,       1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          4096,       1000000,    0,      0,

    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 13),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 14),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 15),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 16),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 17),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 18),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 19),  1000000,    0,      0,

    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 20),  100000,     0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 21),  100000,     0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 22),  100000,     0,      0,

    // define random size and repeated alloca
    ST_RANDOM_REPEATED, ALLOC_WAYS_NONE,        0,          0,          0,          0,      0,

    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          256,        1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          512,        1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          1024,       1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          2048,       1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          4096,       1000000,    0,      0,

    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 13),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 14),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 15),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 16),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 17),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 18),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 19),  1000000,    0,      0,

    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 20),  100000,     0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 21),  100000,     0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 22),  100000,     0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 23),  100000,     0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 24),  100000,     0,      0,

    // define random size and repeated alloca
    ST_RANDOM_CONTIGUOUS, ALLOC_WAYS_NONE,      0,          0,          0,          0,      0,

    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          256,        1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          512,        1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          1024,       1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          2048,       1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          4096,       1000000,    0,      0,

    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          (1 << 13),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          (1 << 14),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          (1 << 15),  1000000,    0,      0,

    // separator
    //ST_SEPARATOR,       ALLOC_WAYS_NONE,        0,          0,          0,          0,      0,
};

test_data_t je_malloc_test_params[] = {
    // define fixed size and repeated alloca
    ST_FIXED_REPEATED,  ALLOC_WAYS_NONE,        0,          0,          0,          0,      0,

    // fixed size and repeated alloca
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      8,          8,          1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      16,         16,         1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      32,         32,         1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      64,         64,         1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      128,        128,        1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      256,        256,        1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      512,        512,        1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      1024,       1024,       1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      2048,       2048,       1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      4096,       4096,       1000000,    0,      0,

    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 13),  (1 << 13),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 14),  (1 << 14),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 15),  (1 << 15),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 16),  (1 << 16),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 17),  (1 << 17),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 18),  (1 << 18),  1000000,    0,      0,

#ifndef _DEBUG
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 19),  (1 << 19),  100000,     0,      0,
                                                
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 20),  (1 << 20),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 21),  (1 << 21),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 22),  (1 << 22),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 23),  (1 << 23),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 24),  (1 << 24),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 25),  (1 << 25),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 26),  (1 << 26),  100000,     0,      0,
#else                                           
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 19),  (1 << 19),  100000,     0,      0,
                                                
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 20),  (1 << 20),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 21),  (1 << 21),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 22),  (1 << 22),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 23),  (1 << 23),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 24),  (1 << 24),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 25),  (1 << 25),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 26),  (1 << 26),  100000,     0,      0,
#endif

    // define continuous size and repeated alloca
    ST_CONTI_REPEATED,  ALLOC_WAYS_NONE,        0,          0,          0,          0,      0,

    // continuous size and repeated alloca
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          256,        1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          512,        1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          1024,       1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          2048,       1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          4096,       1000000,    0,      0,

    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 13),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 14),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 15),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 16),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 17),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 18),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 19),  1000000,    0,      0,

    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 20),  100000,     0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 21),  100000,     0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 22),  100000,     0,      0,

    // define random size and repeated alloca
    ST_RANDOM_REPEATED, ALLOC_WAYS_NONE,        0,          0,          0,          0,      0,

    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          256,        1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          512,        1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          1024,       1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          2048,       1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          4096,       1000000,    0,      0,

    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 13),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 14),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 15),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 16),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 17),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 18),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 19),  1000000,    0,      0,

    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 20),  100000,     0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 21),  100000,     0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 22),  100000,     0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 23),  100000,     0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 24),  100000,     0,      0,

    // define random size and repeated alloca
    ST_RANDOM_CONTIGUOUS, ALLOC_WAYS_NONE,      0,          0,          0,          0,      0,

    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          256,        1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          512,        1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          1024,       1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          2048,       1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          4096,       1000000,    0,      0,

    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          (1 << 13),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          (1 << 14),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          (1 << 15),  1000000,    0,      0,

    // separator
    //ST_SEPARATOR,       ALLOC_WAYS_NONE,        0,          0,          0,          0,      0,
};

test_data_t apr_malloc_test_params[] = {
    // define fixed size and repeated alloca
    ST_FIXED_REPEATED,  ALLOC_WAYS_NONE,        0,          0,          0,          0,      0,

    // fixed size and repeated alloca
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      8,          8,          1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      16,         16,         1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      32,         32,         1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      64,         64,         1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      128,        128,        1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      256,        256,        1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      512,        512,        1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      1024,       1024,       1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      2048,       2048,       1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      4096,       4096,       1000000,    0,      0,

    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 13),  (1 << 13),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 14),  (1 << 14),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 15),  (1 << 15),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 16),  (1 << 16),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 17),  (1 << 17),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 18),  (1 << 18),  1000000,    0,      0,

#ifndef _DEBUG
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 19),  (1 << 19),  1000000,    0,      0,
                                                
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 20),  (1 << 20),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 21),  (1 << 21),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 22),  (1 << 22),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 23),  (1 << 23),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 24),  (1 << 24),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 25),  (1 << 25),  1000000,    0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 26),  (1 << 26),  1000000,    0,      0,
#else                                           
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 19),  (1 << 19),  100000,     0,      0,
                                                
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 20),  (1 << 20),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 21),  (1 << 21),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 22),  (1 << 22),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 23),  (1 << 23),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 24),  (1 << 24),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 25),  (1 << 25),  100000,     0,      0,
    ST_FIXED_SIZE,      AW_REPEATED_ALLOC,      (1 << 26),  (1 << 26),  100000,     0,      0,
#endif

    // define continuous size and repeated alloca
    ST_CONTI_REPEATED,  ALLOC_WAYS_NONE,        0,          0,          0,          0,      0,

    // continuous size and repeated alloca
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          256,        1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          512,        1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          1024,       1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          2048,       1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          4096,       1000000,    0,      0,

    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 13),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 14),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 15),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 16),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 17),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 18),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 19),  1000000,    0,      0,

    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 20),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 21),  1000000,    0,      0,
    ST_CONTINUOUS_SIZE, AW_REPEATED_ALLOC,      1,          (1 << 22),  1000000,    0,      0,

    // define random size and repeated alloca
    ST_RANDOM_REPEATED, ALLOC_WAYS_NONE,        0,          0,          0,          0,      0,

    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          256,        1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          512,        1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          1024,       1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          2048,       1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          4096,       1000000,    0,      0,

    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 13),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 14),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 15),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 16),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 17),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 18),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 19),  1000000,    0,      0,

    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 20),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 21),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 22),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 23),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_REPEATED_ALLOC,      1,          (1 << 24),  1000000,    0,      0,

    // define random size and repeated alloca
    ST_RANDOM_CONTIGUOUS, ALLOC_WAYS_NONE,      0,          0,          0,          0,      0,

    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          256,        1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          512,        1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          1024,       1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          2048,       1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          4096,       1000000,    0,      0,

    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          (1 << 13),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          (1 << 14),  1000000,    0,      0,
    ST_RANDOM_SIZE,     AW_CONTIGUOUS_ALLOC,    1,          (1 << 15),  1000000,    0,      0,

    // separator
    //ST_SEPARATOR,       ALLOC_WAYS_NONE,        0,          0,          0,          0,      0,
};

char *format_bytes_size(char *buf, size_t size, int len)
{
    double fnum;
    size_t num;
    int i, rest_len;
    char fmt[64], unit[64];
    const unsigned int KB_ = 1 << 10;
    const unsigned int MB_ = 1 << 20;
    const unsigned int GB_ = 1 << 30;

    unit[0] = '\0';
    if (buf != NULL) {
        // if size is power of 2 or equal 0
        if ((size & (size - 1)) == 0) {
            if (size >= KB_) {
                if (size >= MB_) {
                    num = size / MB_;
                    strcpy(unit, "MB");
                }
                else {
                    num = size / KB_;
                    strcpy(unit, "KB");
                }
                sprintf(buf, "%3d %s", num, unit);
            }
            else {
                sprintf(fmt, "%%%dd", 3);
                sprintf(buf, fmt, size);
            }
        }
        else {
            if (size >= KB_) {
                if (size >= MB_) {
                    fnum = (double)size / (double)MB_;
                    strcpy(unit, "MB");
                }
                else {
                    fnum = (double)size / (double)KB_;
                    strcpy(unit, "KB");
                }
                sprintf(buf, "%03.1f %s", fnum, unit);
            }
            else {
                sprintf(fmt, "%%%dd", 3);
                sprintf(buf, fmt, size);
            }
        }
        rest_len = len - strlen(buf);
        if (rest_len > 0) {
            for (i = 0; i < rest_len; i++) {
                strcat(buf, " ");
            }
        }
        return buf;
    }
    else
        return "";
}

template<typename T>
double mempool_malloc_tester(mempool_tester_base<T> *tester, test_data_t *test_params, int test_length)
{
    int i;
    double used_time, total_time, fmultiple;
    char buf[128];

    srand(FIXED_RANDOM_SEED);
    total_time = 0.0;

    if (tester != NULL && test_params != NULL) {
        if (test_length > 1) {
            // setting params
            tester->Setting(test_params[1].size_type, test_params[1].alloc_way,
                test_params[1].min_alloc_size, test_params[1].max_alloc_size,
                test_params[1].loop_count1, test_params[1].loop_count2, test_params[1].loop_count3);
            tester->Start();
            tester->Run();
            tester->Stop();
            used_time = tester->GetUsedTime();
            total_time += used_time;
            total_time -= total_time;
        }
        if (g_lang_id == LANG_ZH_CN) {
            mempool_printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
            mempool_printf("  �ڴ���亯��: %s\n", tester->GetFuncName());
            mempool_printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n");
            for (i = 0; i < test_length; i++) {
                if (test_params[i].size_type == ST_SEPARATOR) {
                    mempool_printf("----------------------------------------------------------------\n");
                    continue;
                }
                else if (test_params[i].size_type == ST_FIXED_REPEATED) {
                    mempool_printf("----------------------------------------------------------------\n");
                    mempool_printf("  ���䷽ʽ: �����С�̶�, �ظ����� (����������ͷ�)\n");
                    mempool_printf("----------------------------------------------------------------\n");
                    continue;
                }
                else if (test_params[i].size_type == ST_CONTI_REPEATED) {
                    mempool_printf("----------------------------------------------------------------\n");
                    mempool_printf("  ���䷽ʽ: �����С����, �ظ����� (����������ͷ�)\n");
                    mempool_printf("----------------------------------------------------------------\n");
                    continue;
                }
                else if (test_params[i].size_type == ST_RANDOM_REPEATED) {
                    mempool_printf("----------------------------------------------------------------\n");
                    mempool_printf("  ���䷽ʽ: �����С���, �ظ����� (����������ͷ�)\n");
                    mempool_printf("----------------------------------------------------------------\n");
                    continue;
                }
                else if (test_params[i].size_type == ST_FIXED_CONTIGUOUS) {
                    mempool_printf("----------------------------------------------------------------\n");
                    mempool_printf("  ���䷽ʽ: �����С�̶�, �ظ����� (�ӳ��ͷ�, ÿ1024�������ͷ�)\n");
                    mempool_printf("----------------------------------------------------------------\n");
                    continue;
                }
                else if (test_params[i].size_type == ST_RANDOM_CONTIGUOUS) {
                    mempool_printf("----------------------------------------------------------------\n");
                    mempool_printf("  ���䷽ʽ: �����С���, �ظ����� (�ӳ��ͷ�, ÿ1024�������ͷ�)\n");
                    mempool_printf("----------------------------------------------------------------\n");
                    continue;
                }
                if (test_params[i].size_type == ST_RANDOM_SIZE) {
                    // random seed
                    srand(test_params[i].max_alloc_size);
                }
                // setting params
                fmultiple = (double)MAX_LOOP_COUNT / (double)test_params[i].loop_count1;
                tester->Setting(test_params[i].size_type, test_params[i].alloc_way,
                    test_params[i].min_alloc_size, test_params[i].max_alloc_size,
                    test_params[i].loop_count1, test_params[i].loop_count2, test_params[i].loop_count3);
                tester->Start();
                tester->Run();
                tester->Stop();
                used_time = tester->GetUsedTime();
                total_time += used_time * fmultiple;
                if (test_params[i].size_type == ST_CONTINUOUS_SIZE
                    || test_params[i].size_type == ST_RANDOM_SIZE) {
                    //mempool_printf("�����С: %d-%-8d    ", tester->min_alloc_size, tester->max_alloc_size);
                    mempool_printf("�����С: %d-%s    ", tester->test_param.min_alloc_size,
                        format_bytes_size(buf, tester->test_param.max_alloc_size, 8));
                }
                else {
                    //mempool_printf("�����С: %-8d    ", tester->min_alloc_size);
                    mempool_printf("�����С: %s    ", format_bytes_size(buf, tester->test_param.min_alloc_size, 8));
                }
                mempool_printf("�ظ�����: %-7d", (int)(tester->test_param.loop_count1 * fmultiple));
                if (test_params[i].loop_count1 < MAX_LOOP_COUNT)
                    mempool_printf(" *  ");
                else
                    mempool_printf("    ");
                mempool_printf("��ʱ: %0.3f ms\n", used_time * fmultiple);
            }
            mempool_printf("----------------------------------------------------------------\n");
            mempool_printf("  %s    ����ʱ: %0.3f ��.\n", tester->GetFuncName(), total_time / 1000.0);
            mempool_printf("\n");
        }
        else {
            mempool_printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-\n");
            mempool_printf("  Alloc Function: %s\n", tester->GetFuncName());
            mempool_printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-\n\n");
            for (i = 0; i < test_length; i++) {
                if (test_params[i].size_type == ST_SEPARATOR) {
                    mempool_printf("-------------------------------------------------------------------------\n");
                    continue;
                }
                else if (test_params[i].size_type == ST_FIXED_REPEATED) {
                    mempool_printf("-------------------------------------------------------------------------\n");
                    mempool_printf("  Alloc Mode: alloc size is fixed,\n");
                    mempool_printf("              repeat alloc (malloc and immediately release)\n");
                    mempool_printf("-------------------------------------------------------------------------\n");
                    continue;
                }
                else if (test_params[i].size_type == ST_CONTI_REPEATED) {
                    mempool_printf("-------------------------------------------------------------------------\n");
                    mempool_printf("  Alloc Mode: alloc size is continuously increase,\n");
                    mempool_printf("              repeat alloc (malloc and immediately release)\n");
                    mempool_printf("-------------------------------------------------------------------------\n");
                    continue;
                }
                else if (test_params[i].size_type == ST_RANDOM_REPEATED) {
                    mempool_printf("-------------------------------------------------------------------------\n");
                    mempool_printf("  Alloc Mode: alloc size is within a certain range of random numbers,\n");
                    mempool_printf("              repeat alloc (malloc and immediately release)\n");
                    mempool_printf("-------------------------------------------------------------------------\n");
                    continue;
                }
                else if (test_params[i].size_type == ST_FIXED_CONTIGUOUS) {
                    mempool_printf("-------------------------------------------------------------------------\n");
                    mempool_printf("  Alloc Mode: alloc size is fixed,\n");
                    mempool_printf("              repeat alloc (malloc 1024 times and batch release)\n");
                    mempool_printf("-------------------------------------------------------------------------\n");
                    continue;
                }
                else if (test_params[i].size_type == ST_RANDOM_CONTIGUOUS) {
                    mempool_printf("-------------------------------------------------------------------------\n");
                    mempool_printf("  Alloc Mode: alloc size is within a certain range of random numbers,\n");;
                    mempool_printf("              repeat alloc (malloc 1024 times and batch release)\n");
                    mempool_printf("-------------------------------------------------------------------------\n");
                    continue;
                }
                if (test_params[i].size_type == ST_RANDOM_SIZE) {
                    // random seed
                    srand(test_params[i].max_alloc_size);
                }
                // setting params
                fmultiple = (double)MAX_LOOP_COUNT / (double)test_params[i].loop_count1;
                tester->Setting(test_params[i].size_type, test_params[i].alloc_way,
                    test_params[i].min_alloc_size, test_params[i].max_alloc_size,
                    test_params[i].loop_count1, test_params[i].loop_count2, test_params[i].loop_count3);
                tester->Start();
                tester->Run();
                tester->Stop();
                used_time = tester->GetUsedTime();
                total_time += used_time * fmultiple;
                if (test_params[i].size_type == ST_CONTINUOUS_SIZE
                     || test_params[i].size_type == ST_RANDOM_SIZE) {
                    //mempool_printf("Alloc Size: %d-%-8d    ", tester->test_param.min_alloc_size, tester->test_param.max_alloc_size);
                    mempool_printf("Alloc Size: %d-%s    ", tester->test_param.min_alloc_size,
                        format_bytes_size(buf, tester->test_param.max_alloc_size, 8));
                }
                else {
                    //mempool_printf("Alloc Size: %-8d    ", tester->test_param.min_alloc_size);
                    mempool_printf("Alloc Size: %s    ", format_bytes_size(buf, tester->test_param.min_alloc_size, 8));
                }
                mempool_printf("Repeat Times: %-7d", (int)(tester->test_param.loop_count1 * fmultiple));
                if (test_params[i].loop_count1 < MAX_LOOP_COUNT)
                    mempool_printf(" *  ");
                else
                    mempool_printf("    ");
                mempool_printf("Used Time: %0.3f ms\n", used_time * fmultiple);
            }
            mempool_printf("-------------------------------------------------------------------------\n");
            mempool_printf("  %s    Total Time: %0.3f seconds.\n", tester->GetFuncName(), total_time / 1000.0);
            mempool_printf("\n");
        }
    }
    return total_time;
}

void Memory_Pool_Test()
{
    int test_length;
    test_data_t *test_params;

    double time_orgi_malloc, time_je_malloc;
    //double time_tc_malloc, time_apr_malloc;

    // original malloc()
    malloc_tester *orgi_tester = new malloc_tester();
    test_length = sizeof(orig_malloc_test_params) / sizeof(test_data_t);
    test_params = &orig_malloc_test_params[0];
    time_orgi_malloc = mempool_malloc_tester<malloc_tester>(orgi_tester, test_params, test_length);
    if (orgi_tester) {
        delete orgi_tester;
        orgi_tester = NULL;
    }

    // je_malloc()
    jemalloc_tester *je_tester = new jemalloc_tester();
    test_length = sizeof(je_malloc_test_params) / sizeof(test_data_t);
    test_params = &je_malloc_test_params[0];
    time_je_malloc = mempool_malloc_tester<jemalloc_tester>(je_tester, test_params, test_length);
    if (je_tester) {
        delete je_tester;
        je_tester = NULL;
    }

    mempool_printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    if (g_lang_id == LANG_ZH_CN)
        mempool_printf("je_malloc() �ٶ���������: %0.2f %%\n", time_orgi_malloc * 100.0 / time_je_malloc);
    else
        mempool_printf("je_malloc() speed up: %0.2f %%\n", time_orgi_malloc * 100.0 / time_je_malloc);
    mempool_printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    mempool_printf("\n");

#if 0

    // tc_malloc()
    tcmalloc_tester *tc_tester = new tcmalloc_tester();
    test_length = sizeof(apr_malloc_test_params) / sizeof(test_data_t);
    test_params = &apr_malloc_test_params[0];
    time_tc_malloc = mempool_malloc_tester<tcmalloc_tester>(tc_tester, test_params, test_length);
    if (tc_tester) {
        delete tc_tester;
        tc_tester = NULL;
    }

    mempool_printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    if (s_lang_id == LANG_ZH_CN)
        mempool_printf("tc_malloc() �ٶ���������: %0.2f %%\n", time_orgi_malloc * 100.0 / time_tc_malloc);
    else
        mempool_printf("tc_malloc() speed up: %0.2f %%\n", time_orgi_malloc * 100.0 / time_tc_malloc);
    mempool_printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    mempool_printf("\n");

    // apr_malloc()
    aprmalloc_tester *apr_tester = new aprmalloc_tester();
    test_length = sizeof(apr_malloc_test_params) / sizeof(test_data_t);
    test_params = &apr_malloc_test_params[0];
    time_apr_malloc = mempool_malloc_tester<aprmalloc_tester>(apr_tester, test_params, test_length);
    if (apr_tester) {
        delete apr_tester;
        apr_tester = NULL;
    }

    mempool_printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    if (s_lang_id == LANG_ZH_CN)
        mempool_printf("apr_malloc() �ٶ���������: %0.2f %%\n", time_orgi_malloc * 100.0 / time_apr_malloc);
    else
        mempool_printf("apr_malloc() speed up: %0.2f %%\n", time_orgi_malloc * 100.0 / time_apr_malloc);
    mempool_printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    mempool_printf("\n");

#endif
}

int main(int argc, char * argv[])
{
    // Get the current user language id
    g_lang_id = get_current_langid();

    // Only for test english display
    //g_lang_id = LANG_EN_US;

    // Make the random seed
    srand((unsigned int)time(NULL));

    mempool_log_init(DEFAULT_LOG_FILENAME);

    // Run the test
    Memory_Pool_Test();

    mempool_printf("\n");

    mempool_log_close();

#if 0

    // If you use jemalloc through the static lib,
    // must be manual initialize jemalloc first.
    je_init();

    void *p = (void *)je_malloc(128);
    if (p) {
        printf("malloc(%u) result ptr = 0x%08X\n\n", 128, p);
        je_free(p);
    }

    p = (void *)je_malloc(256);
    if (p) {
        printf("malloc(%u) result ptr = 0x%08X\n\n", 256, p);
        je_free(p);
    }

    mempool_tester_base<jemalloc_tester> *tester;
    mempool_tester<jemalloc_tester> *tester2;
    jemalloc_tester *je_tester;

    tester = static_cast<mempool_tester_base<jemalloc_tester> *>(new mempool_tester_base<jemalloc_tester>());
    if (tester) {
        printf("GetFuncName() = \"%s\"\n", tester->GetFuncName());
        tester->Run();
        //delete tester;
        //tester = NULL;
    }

    printf("\n");

    tester2 = static_cast<mempool_tester<jemalloc_tester> *>(tester);
    if (tester2) {
        printf("GetFuncName() = \"%s\"\n", tester2->GetFuncName());
        tester2->Run();
        //delete tester;
        //tester = NULL;
    }

    printf("\n");

    je_tester = static_cast<jemalloc_tester *>(tester);
    if (je_tester) {
        printf("GetFuncName() = \"%s\"\n", je_tester->GetFuncName());
        je_tester->Run();
        //delete je_tester;
    }

    if (tester) {
        delete tester;
        tester = NULL;
    }

    // Unload the jemalloc
    je_uninit();

#endif

    printf("\n");
#ifdef _WIN32
    system("pause");
#endif
	return 0;
}
