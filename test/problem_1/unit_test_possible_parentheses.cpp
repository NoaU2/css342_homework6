/*
 * DO NOT MAKE ANY CHANGES
 */

#include "problem_1/catalan_number_solver.h"
#include "unit_test_possible_parentheses.h"
#include "unit_test_utils.h"


// this test will take some time to run for larger SIZE
// SIZE == 15 took about 17 sec
TEST(problem_1, possible_parentheses) {
    const size_t SIZE = 12;
    //runs for n 0 through 12
    for (size_t num_of_pair = 0; num_of_pair < SIZE; num_of_pair++) {
        //creates a result vector and a result set
        std::vector<std::string> results;
        std::set<std::string> result_set;

        CatalanNumberSolver::possible_parenthesis(num_of_pair, results);

        // verify result size
        size_t expect_size = CatalanNumberSolver::catalan_number(num_of_pair);
        ASSERT_EQ(expect_size, results.size());

        // verify result correctness & uniqueness
        for (const std::string &result: results) {
            //verifies that the parenthesis string is valid
            ASSERT_TRUE(is_valid(result));
            //verifies that it is unique
            ASSERT_TRUE(result_set.find(result) == result_set.end());
            result_set.insert(result);
        }
    }
}