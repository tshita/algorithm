/*
  Range Sum Query Problem (using Fenwick Tree)
  ============================================

  # Problem
    Input: 数列 (d_0, d_1, ..., d_{n-1})
    Query:
      1. 区間追加: d_i += w, i \in [l, r)
      2. 接頭辞和: d_0 + d_1 + ... + d_{i-1}
      3. 区間和： d_l + d_{l+1} + ... + d_{r-1}

  # Complexity
    - Time:
      + 区間追加: O(log n)
      + 接頭辞和: O(log n)
      + 区間和: O(log n)
    - Space: 2 * (n + 1) + 1

  # Usage
    - RangeSumQuery<T> rsq(n): 型T で要素数 n の列を作成（各要素は 0 で初期化）
    - rsq.add(l, r, v): d_i += v (i \in [l, r))
    - rsq.prefix(i): d_0 + d_1 + ... + d_{i-1} を返す
    - rsq.sum(l, r): d_l + d_{l+1} + ... + d_{r-1} を返す

  # Description
    Range Sum Query を2つの Fenwick Tree で解く．

  # Note
    - Fenwick Tree の各要素に添字と値の積が代入されるのでオーバーフローに注意

  # References
    [保坂和宏，Binary Indexed Treeのはなし（2018年8月26日アクセス）]
    (http://hos.ac/slides/20140319_bit.pdf )

  # Verified
    - [POJ3468 A Simple Problem with Integers]
      (http://poj.org/problem?id=3468)
*/

#include <iostream>
#include <vector>

// -------------8<------- start of library -------8<------------------------
template<typename T>
struct RangeSumQuery {
    std::size_t sz;
    std::vector<T> d1, d2;

    explicit RangeSumQuery() : sz(0) {}
    explicit RangeSumQuery(int _n) : sz(_n + 1), d1(sz), d2(sz) {}

    void add(std::size_t l, std::size_t r, const T &value) {
        for (std::size_t i = l; i < sz; i |= i + 1) d1[i] -= value * l;
        for (std::size_t i = r; i < sz; i |= i + 1) d1[i] += value * r;
        for (std::size_t i = l; i < sz; i |= i + 1) d2[i] += value;
        for (std::size_t i = r; i < sz; i |= i + 1) d2[i] -= value;
    }

    T prefix(int idx) const {
        T res1 = 0, res2 = 0;
        for (int i = idx - 1; i >= 0; i = (i & (i + 1)) - 1) res1 += d1[i];
        for (int i = idx - 1; i >= 0; i = (i & (i + 1)) - 1) res2 += d2[i];
        return res1 + res2 * idx;
    }

    T sum(const int l, const int r) const { return prefix(r) - prefix(l); }
};
// -------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    int n, q;
    std::cin >> n >> q;

    RangeSumQuery<int> rsq(n);
    for (int i = 0, com, x, y; i < q; ++i) {
        std::cin >> com >> x >> y;
        if (com == 0) rsq.add(x - 1, x, y);
        else if (com == 1) std::cout << rsq.sum(x - 1, y) << '\n';
    }

    return 0;
}
