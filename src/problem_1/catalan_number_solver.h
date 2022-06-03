/*
 * DO NOT MAKE ANY CHANGES
 */

#include <vector>
#include <string>

class CatalanNumberSolver {
public:
    static size_t catalan_number(size_t n);

    static void possible_parenthesis(size_t n, std::vector<std::string> &result);

    static void backtrack(size_t n, std::vector<std::string> &result, std::string &current,
                          int closed, int open);
};