/*
  Fenwick Tree [Peter M. Fenwick 1994] (add a single element / accumulate an prefix)
  フェニック木（点追加・接頭辞の畳み込み）
  ===================================================================================

  # Problem
    Input: 可換モノイド（M, *）上の列 (d_0, d_1, ..., d_{n-1})
    Query:
      1. 点変更: d_i に w を加える (d_i = d_i * w)
      2. 接頭辞の畳み込み: d_0 + d_1 + ... + d_i を求める

      3. 区間畳み込み： d_l * d_{l+1} * ... * d_r （(M, *) が可換群のときのみ）

    Def. モノイド（monoid）
      集合 M と M 上の２項演算子 * の組 (M, *) が以下の条件を満たすときモノイドと呼ぶ
      　・結合律：任意の x, y, z \in M に対して，(x * y) * z = x * (y * z)
      　・単位元の存在：ある e \in M が存在して，任意の x \in M に対して x * e = e * x = x

      - 交換律を満たすモノイドを可換モノイド（commutative monoid）と呼ぶ
      - 二項演算子が結合律のみを満たす場合は (M, *) を半群（semigroup）と呼ぶ
      - モノイドに逆元の存在を加えたものを群（group）と呼ぶ
      - 交換律を満たす群を可換群（commutative group）またはアーベル群（abelian group）と呼ぶ
        + 交換律：任意の x, y \in M に対して x * y = y * x
        + 逆元の存在：任意の x \in M に対して，ある y \in M が存在して  x * y = y * x = e

      e.g. (R, max, -inf), (R, min, inf) は可換モノイドで (R, +, 0) は可換群
      　　　（R は実数全体，3番目の要素は単位元）

  # Complexity
    - Time:
      + 点変更: O(log n)
      + 接頭辞の畳み込み: O(log n)
      + 区間の畳み込み: O(log n)
      + 初期化: O(n)
    - Space: O(n)

  # Usage
    二項演算子が * で単位元が 0 の T型の要素からなる代数系を (T, *, 0) と表すことにする

    - FenwickTree<commutative_monoid<T>> bit:
        T型の可換モノイドからなる空の Fenwick Tree を作成
    - FenwickTree<commutative_monoid<T>> bit(n):
        T型の可換モノイドからなる要素数 Fenwick Tree を作成（単位元で初期化）
        + min_commutative_monoind<T>:
          可換モノイド (T, min, max(T))  (Range Minimum Query)
        + max_commutative_monoind<T>:
          可換モノイド (T, max, min(T))  (Range Maximum Query)
        + sum_commutative_monoind<T>:
          可換群 (T, +, 0) (Range Sum Query)
          要素の型が T，二項演算子 +，単位元は 0 とする可換モノイド（Range Sum Query）

    - bit.resize(n, v): bit のサイズを n に変更して，全ての要素を v に初期化
    - bit.initialize(data): コンテナクラスのオブジェクト data で初期化

    - bit.add(i, v): d_i = d_i * v
    - bit.prefix(i): d_0 * d_1 * ... d_i を返す
    - bit.accumulate(l, r): d_l * d_{l+1} * ... * d_r を返す（可換群のみ）

  # Description
    フェニック木は可換モノイド上の列に対して，値の追加（d_i *= d_i）と
    接頭辞の畳み込み（d_0 * d_1 * ... * d_i）がそれぞれ O(log n) 時間で求まるデータ構造．
    区間の畳み込み（d_l * ... d_r）は可換群のみなので，非可換モノイドに対して行いたいときは
    セグメント木を使うのがよい．

    TODO: bit.resize(n, v) のメソッド名を bit.initialize(n, v) にしたいが，
    　　　　テンプレートの方が呼び出されるので修正したい．

  # Note
    - Binary Indexed Tree とも呼ばれる
    - 0-index の添字でで実装を行っている
    - 平衡二分探索木やSegment Treeよりも定数倍の意味で高速（bit演算と少ない領域）
    - 区間畳み込みは可換群のみなので (R, max) と (R, min) には適用できない

  # References
    [保坂和宏，Binary Indexed Treeのはなし（2018年8月26日アクセス）]
    (http://hos.ac/slides/20140319_bit.pdf )

  # Verified
    [AOJ DSL-DSL_2_B Range Query]
    (http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=DSL_2_B&lang=jp )
*/

#include <iostream>
#include <vector>
#include <limits>
#include <iterator>
#include <algorithm>

// -------------8<------- start of library -------8<------------------------
template <typename CommutativeMonoid>
struct FenwickTree {
    using CM = CommutativeMonoid;
    using T = typename CommutativeMonoid::value_type;

    std::size_t sz;
    std::vector<T> d;

    explicit FenwickTree() : sz(0) {}
    explicit FenwickTree(int _n) : sz(_n), d(_n, CM::unit()) {}

    void resize(const std::size_t n, const T &value = CM::unit()) {
        sz = n;
        d.resize(sz, value);
        setup();
    }

    template<typename InputIterator>
    void initialize(InputIterator first, InputIterator last) {
        sz = std::distance(first, last);
        d.resize(sz, CM::unit());
        std::copy(first, last, d.begin());
        setup();
    }

    void setup() {
        for (std::size_t i = 0; i < sz; ++i)
            if ((i | (i + 1)) < sz)
                d[i | (i + 1)] = CM::op(d[i | (i + 1)], d[i]);
    }

    void add(std::size_t idx, const T &value = CM::unit()) {
        for ( ; idx < sz; idx |= idx + 1) d[idx] = CM::op(d[idx], value);
    }

    T prefix(int idx) const {
        T res = CM::unit();
        for ( ; idx >= 0; idx = (idx & (idx + 1)) - 1)
            res = CM::op(res, d[idx]);
        return res;
    }

    // only accumulate group
    T accumulate(const int l, const int r) const { return prefix(r) - prefix(l - 1); }
};

template<typename T>
struct min_commutative_monoid {
    using value_type = T;
    static constexpr T unit() { return std::numeric_limits<T>::max(); }
    static T op(const T &lhs, const T &rhs) { return std::min(lhs, rhs); }
};

template<typename T>
struct max_commutative_monoid {
    using value_type = T;
    static constexpr T unit() { return std::numeric_limits<T>::min(); }
    static T op(const T &lhs, const T &rhs) { return std::max(lhs, rhs); }
};

template<typename T>
struct sum_commutative_group {
    using value_type = T;
    static constexpr T unit() { return 0; }
    static T op(const T &lhs, const T &rhs) { return lhs + rhs; }
};
// -------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    int n, q;
    std::cin >> n >> q;

    FenwickTree<sum_commutative_group<int>> bit(n);
    for (int i = 0, com, x, y; i < q; ++i) {
        std::cin >> com >> x >> y;
        if (com == 0) bit.add(x - 1, y);
        else if (com == 1) std::cout << bit.accumulate(x - 1, y - 1) << '\n';
    }

    return 0;
}
