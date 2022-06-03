#include "catalan_number_solver.h"



void CatalanNumberSolver::possible_parenthesis(size_t n, std::vector<std::string> &result) {

    std::string current = "";
    backtrack(n, result, current, (int)n, (int)n);

}
//https://leetcode.com/problems/generate-parentheses/discuss/2103310/C%2B%2B-0ms-easy-solution-with-line-to-line-comments
//used to understand the problem ^^^^
void CatalanNumberSolver::backtrack(size_t n, std::vector<std::string> &result, std::string &current,
                                    int closed, int open){

    if(closed == 0 && open == 0){
        result.push_back(current);
        return;
    }

    if(open > 0){
        current.push_back('(');
        backtrack(n, result, current, closed, open - 1);
        current.pop_back();
    }

    if(closed > 0 && open < closed){
        current.push_back(')');
        backtrack(n, result, current, closed - 1, open);
        current.pop_back();
    }

}


size_t CatalanNumberSolver::catalan_number(size_t n) {
    if (n < 2) {
        return 1;
    }
    size_t numerator = 1, denominator = 1;

    for (size_t k = 2; k <= n; k++) {
        numerator *= (n + k);
        denominator *= k;
    }

    return numerator / denominator;
}
