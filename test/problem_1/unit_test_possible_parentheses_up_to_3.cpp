#include "problem_1/catalan_number_solver.h"
#include "unit_test_possible_parentheses.h"
#include "unit_test_utils.h"


TEST(problem_1, your_test) {

    //created set of sets for the expected strings
    std::set<std::set<std::string>> expected = {{{"()()()"} , {"(())()"} , {"()(())"} , {"((()))"} , {"(()())"}},  {{"()()"}, {"(())"}},{{"()"}}};
    int index = 3;
    std::vector<std::string> actual;

    for(const std::set<std::string> &expect: expected){
        CatalanNumberSolver::possible_parenthesis(index, actual);
        ASSERT_EQ(actual.size(), expect.size());
        for(const std::string &parenthesisPermutation: actual){
            //check if the parenthesis are valid + checks if the string is found in the expected list
            ASSERT_TRUE(is_valid(parenthesisPermutation));
            ASSERT_TRUE(expect.count(parenthesisPermutation) == 1);
        }
        index--;
        actual.clear();
    }

}
