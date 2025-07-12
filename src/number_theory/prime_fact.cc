/*
  Prime factorization (Trial division)
  素因数分解（自明な分割）
  ====================================

  # Problem
    Input: 正整数 n
    Output: n の素因数分解 n = p_1^e_1 * p_2^e_2 * ... * p_k^e_k
    　　　　　(p_1, ..., p_k は互いに異なる素数)

  # Complexity
    Time: O(sqrt(n))
    Space: O(log(n))

  # Description
    2 から sqrt(n) まで順番に n を割っていく

  # References
    - [Wiki. Trial division](https://en.wikipedia.org/wiki/Trial_division)

  # Verified
    - [AOJ NTL_1_A](udge.u-aizu.ac.jp/onlinejudge/description.jsp?id=NTL_1_A)
*/

#include <iostream>
#include <vector>
#include <tuple>

// -------------8<------- start of library -------8<------------------------
template<class T>
std::vector<std::pair<T, T>> PrimeFactorization(T n) {
    std::vector<std::pair<T, T>> pf;
    T m = n;
    for (T i = 2; i * i <= n; ++i) {
        if (m % i != 0) continue;
        T cnt = 0;
        while (m % i == 0) { ++cnt; m /= i; }
        pf.emplace_back(std::make_pair(i, cnt));
    }
    if (1 < m) pf.emplace_back(std::make_pair(m, 1));

    return pf;
}
// -------------8<------- end of library ---------8-------------------------

int main() {
    int n;
    std::cin >> n;
    std::cout << n << ":";

    auto pf = PrimeFactorization<int>(n);
    for (auto p : pf) {
        for (int i = 0; i < std::get<1>(p); ++i)
            std::cout << ' ' << std::get<0>(p);
    }
    std::cout << std::endl;

    return 0;
}
