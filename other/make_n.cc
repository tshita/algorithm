/*

Make N
==============================================================================

# Problem
- Input: 整数 m, a_1, a_2, ..., a_n
- Output: a_1, a_2, ..., a_n と四則演算と括弧を用いて m と等しくなる式を見つける（逆ポーランド記法を出力）

例）13, 2, 5, 5, 9 => (9 - 5) * 2 + 5

# Complexity
- Time: O(n! * C_n * 4^n) （ただし、C_n は n 番目のカタラン数）
    - n が十分大きいとき C_n = 4^n / (n^{3 / 2} * sqrt(PI)) となるので大雑把に O(n! * 4^n)

# Description
整数 a_1, a_2, ..., a_n の順番が固定されて演算子がひとつの場合の計算順序は () を正しく並べる方法と等しく、その組合せ数はカタラン数に等しい。また、逆ポーランド記法の並べ方にも等しい。
　例）((a * b) * c) * (d * e) = (())() = ab*c*de**
逆ポーランド記法を列挙するには一対一対応している格子上の最短経路数え上げを行う。n x n の格子の (0, 0) から (n - 1, n - 1) までの最短経路で途中の座標 (i, j) が j <= i を満たすようなものを考える。最短経路で右に行くのを被演算子の選択、上に行くのを演算子の選択とすると対応する。ただし、逆ポーランド記法では最初は被演算子で始まるので a_1 を除いて a_2 から考える。
　例）ab*c*de** は (0, 0) > (1, 0) > (1, 1) > (2, 1) > (2, 2) > (3, 2) > (4, 2) > (4, 3) > (4, 4)
逆ポーランド記法が列挙できたら、あとは被演算子の順列の列挙（n! 通り）と n - 1 個の演算子を四則演算から選ぶ（4^n 通り）を試せばよい。

# Note
- 一桁の数字 4 つの場合はテンパズル（10 Puzzle）またはメイクテン（make 10）と呼ばれているらしいが、一般の場合になんと呼べばいいのかわからなかったので Make N にした。
- 和と積が可換で結合的であることを用いて重複を除外して被演算子を列挙するとコンパクトで高速になるのかなと思ったけど大変そうだしあまり効果なさそう？

# Reference
 - [カタラン数の意味と漸化式＠高校数学の美しい物語](https://manabitimes.jp/math/657)
 - [QuizKnock コンピュータを使ってもパズル王に勝ちたい！！＠YouTube](https://www.youtube.com/watch?v=4mh9qsH0Zhs&t=135s)

*/

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
    std::stack<std::variant<rational, char>> polish_notation;

    bool find_all_equations = true;
    std::vector<std::string> equations;

    void FindSolution();
    bool Rec(const int idx_op, const int idx_nums);
    bool CorrectEquation(std::stack<std::variant<rational, char>> pn);
    void SetEquation(std::stack<std::variant<rational, char>> pn);

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
        for (const std::string &eq_i : equations) { std::cout << "(" << no++ << ") " << eq_i << '\n'; }
        std::cout << delimiter;
    }
};

void MakeN::FindSolution() {
    equations.clear();

    do {
        polish_notation = {};
        if (Rec(0, 0) && !find_all_equations) break;
    } while (std::next_permutation(nums.begin(), nums.end()));
}

bool MakeN::CorrectEquation(std::stack<std::variant<rational, char>> pn) {
    std::stack<rational> operand;
    while (pn.size() != 1 || pn.top().index() != 0) {
        if (pn.top().index() == 0) {
            operand.push(std::get<0>(pn.top()));
            pn.pop();
        }
        else {
            // a @ b (@ \in {+, -, *, /})
            const rational b = operand.top(); operand.pop();
            const rational a = operand.top(); operand.pop();
            char op = std::get<1>(pn.top()); pn.pop();
            if (op == '+') pn.push(a + b);
            else if (op == '-') pn.push(a - b);
            else if (op == '*') pn.push(a * b);
            else if (op == '/' && b != 0) pn.push(a / b);
            else return false;
        }
    }
    return (std::get<0>(pn.top()) == target);
}

void MakeN::SetEquation(std::stack<std::variant<rational, char>> pn) {
    std::string equation;
    while (!pn.empty()) {
        if (pn.top().index() == 0) equation += to_string(std::get<0>(pn.top())) + " ";
        else equation += std::string(1, std::get<1>(pn.top())) + " ";
        pn.pop();
    }
    equations.emplace_back(equation);
}

bool MakeN::Rec(const int idx_op, const int idx_nums) {
    if (idx_op + 1 == (int)nums.size() && idx_nums + 1 == (int)nums.size()) {
        polish_notation.push(nums.front());

        if (CorrectEquation(polish_notation)) {
            SetEquation(polish_notation);
            polish_notation.pop();
            return true;
        }

        polish_notation.pop();
        return false;
    }

    if (idx_op + 1 < (int)nums.size()) {
        for (const char &op : std::string("+-*/")) {
            polish_notation.push(op);
            if (Rec(idx_op + 1, idx_nums) && !find_all_equations) return true;
            polish_notation.pop();
        }
    }
    if (idx_nums + 1 <= idx_op) {
        polish_notation.push(nums[idx_nums + 1]);
        if (Rec(idx_op, idx_nums + 1) && !find_all_equations) return true;
        polish_notation.pop();
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
