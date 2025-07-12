#include <iostream>
#include <vector>
#include <stack>
#include <variant>
#include <optional>

#include <boost/rational.hpp>

// ---------------------8<------- start of library -------8<--------------------
using rational = boost::rational<long long>;

class MakeN {
    std::optional<rational> target;
    std::vector<rational> nums;

    std::stack<rational> st_num;
    std::stack<std::pair<unsigned char, char>> st_op;

    bool find_all_equations = true;
    std::vector<std::string> equations;

    void FindSolution();
    bool Rec(const int idx_nums, const int idx_op);
    void SetEquation(std::stack<std::pair<unsigned char, char>> st_op);

    std::string to_string(const rational &r) const {
        if (r.denominator() == 1) return std::to_string(r.numerator());
        else return std::to_string(r.numerator()) + " / " + std::to_string(r.denominator());
    }

public:
    MakeN() {}
    MakeN(const rational &_t, std::vector<rational> &_ns) : target(_t), nums(_ns) { FindSolution(); }

    void clear() { target = std::nullopt; nums.clear(); }
    void set(rational _t, std::vector<rational> _ns) { clear(); set_target(_t); set_nums(std::move(_ns)); }
    void set_nums(std::vector<rational> _ns) {
         nums = std::move(_ns);
         std::sort(nums.begin(), nums.end()); // 全ての順列を取得する場合には昇順にソート済みである必要がある
         if (nums.size() > 0 && target) FindSolution();
    }
    void set_target(rational _t) {
        target = _t;
        if (nums.size() > 0) FindSolution();
    }

    void PrintEquations(const char delimiter = '\0') const {
        std::cout << to_string(*target) << " = \n";
        unsigned int no = 1;
        for (const auto &eq_i : equations) { std::cout << "(" << no++ << ") " << eq_i << '\n'; }
        std::cout << delimiter;
    }
};

void MakeN::FindSolution() {
    equations.clear();

    do {
        st_op = {}; st_num = {};
        st_num.push(nums.front());
        if (Rec(0, 0) && !find_all_equations) break;
    } while (std::next_permutation(nums.begin(), nums.end()));
}

void MakeN::SetEquation(std::stack<std::pair<unsigned char, char>> st_op) {
    std::string equation;

    int idx_num = nums.size() - 1;
    for (int i = 2 * nums.size() - 2; 0 <= i; --i) {
        if (!st_op.empty() && st_op.top().first == i) {
            equation = std::string(1, st_op.top().second) + " " + equation;
            st_op.pop();
        }
        else {
            equation = to_string(nums[idx_num--]) + " " + equation;
        }
    }

    equations.emplace_back(equation);
}

bool MakeN::Rec(const int idx_nums, const int idx_op) {
    if (idx_nums + 1 == (int)nums.size() && idx_op + 1 == (int)nums.size()) {
        if (*target == st_num.top()) {
            SetEquation(st_op);
            return true;
        }
        return false;
    }

    if (idx_op < idx_nums) {
        const rational b = st_num.top(); st_num.pop();
        const rational a = st_num.top(); st_num.pop();

        for (const char &op : std::string("+-*/")) {
            if (op == '+') st_num.push(a + b);
            else if (op == '-') st_num.push(a - b);
            else if (op == '*') st_num.push(a * b);
            else if (op == '/') {
                if (b == 0) continue;
                st_num.push(a / b);
            }

            st_op.push(std::make_pair(idx_nums + idx_op + 1, op));
            if (Rec(idx_nums, idx_op + 1) && !find_all_equations) return true;

            st_op.pop(); st_num.pop();
        }
        st_num.push(a); st_num.push(b);
    }
    if (idx_nums + 1 < (int)nums.size()) {
        st_num.push(nums[idx_nums + 1]);
        if (Rec(idx_nums + 1, idx_op) && !find_all_equations) return true;
        st_num.pop();
    }

    return false;
}
// ---------------------8<------- end of library   -------8<---------------------


int main() {
    MakeN solver;

    // ref. QuizKnock（ https://www.youtube.com/watch?v=4mh9qsH0Zhs&t=135s ）
    solver.set(13, {2, 5, 5, 9});
    solver.PrintEquations('\n'); // 2 9 5 - * 5 +

    solver.set(23, {4, 8, 8, 9});
    solver.PrintEquations('\n'); // 8 4 - 8 * 9 -

    solver.set(24, {3, 5, 8, 9});
    solver.PrintEquations('\n'); // 3 9 * 8 - 5 +

    solver.set(19, {1, 3, 5, 7});
    solver.PrintEquations('\n'); // 3 5 1 - * 7 +

    solver.set(16, {1, 2, 2, 6, 7, 9});
    solver.PrintEquations('\n'); // 1 9 7 6 2 / - 2 + + +

    solver.set(13, {1, 2, 3, 3, 9});
    solver.PrintEquations('\n'); // 1 9 3 3 / 2 + + +

    solver.set(9, {1, 4, 5, 6, 6, 6});
    solver.PrintEquations('\n'); // 1 6 6 6 / 5 4 - + + +

    solver.set(13, {1, 5, 6, 7});
    solver.PrintEquations('\n'); // 1 7 5 - 6 * +

    solver.set(24, {1, 2, 3, 6, 6});
    solver.PrintEquations('\n'); // 1 6 * 6 3 + 2 * +

    solver.set(19, {2, 3, 4, 4});
    solver.PrintEquations('\n'); // 2 4 4 + * 3 +

    solver.set(14, {2, 4, 7, 9, 9});
    solver.PrintEquations('\n'); // 2 9 9 / 7 4 + + +

    solver.set(8, {3, 4, 7, 8, 8, 9});
    solver.PrintEquations('\n'); // 3 9 8 8 - 7 * 4 - + +

    solver.set(9, {1, 3, 4, 6});
    solver.PrintEquations('\n'); // 1 6 4 3 / * +

    solver.set(14, {2, 3, 9, 9});
    solver.PrintEquations('\n'); // 2 9 9 3 / + +

    // I'm tired to try all problems... (to 5:35)

    // pick up some problems
    solver.set(18, {1, 6, 8, 8});
    solver.PrintEquations('\n'); // 8 6 - 1 8 + *

    solver.set(19, {5, 6, 7, 9});
    solver.PrintEquations('\n'); // 6 9 * 7 5 * -


    return 0;
}
