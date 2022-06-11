#include "unit_test_db_perf_range_query.h"

TEST(problem_4, perf_range_query) {

    const int MIN_HOBBY_COUNT = 5;
    const int HOBBY_TYPE_COUNT = 50;
    const int AGE_MAX = 300;

    const int MAX_TABLE_SIZE = 1000;
    const int STEP_TABLE_SIZE = 1000;

    for (unsigned table_size = 0; table_size <= MAX_TABLE_SIZE; table_size += STEP_TABLE_SIZE) {
        run_index_perf_range_query(table_size, MIN_HOBBY_COUNT, HOBBY_TYPE_COUNT, AGE_MAX);
    }
}