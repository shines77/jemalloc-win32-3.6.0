
#include "jemalloc_tester.h"

jemalloc_tester::jemalloc_tester() : mempool_tester_base<jemalloc_tester>()
{
    je_init();
}

jemalloc_tester::jemalloc_tester(size_types size_type, alloc_ways alloc_way,
                                 int min_alloc_size, int max_alloc_size,
                                 int loop_count1, int loop_count2 /* = 0 */,
                                 int loop_count3 /* = 0 */)
 : mempool_tester_base<jemalloc_tester>(size_type, alloc_way,
                                        min_alloc_size, max_alloc_size,
                                        loop_count1, loop_count2, loop_count3)
{
    je_init();
}

jemalloc_tester::~jemalloc_tester()
{
    // Do nothing!
    je_uninit();
}
