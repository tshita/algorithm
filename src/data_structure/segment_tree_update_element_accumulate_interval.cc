/*
  Segment Tree (update a single element / accumulate an interval)
  区分木（点変更・区間畳み込み）
  ==============================================================

  # Problem
    Input: モノイド (M, *) 上の列 (d_0, d_1, ..., d_{n-1})
    Query:
      ・点変更： d_i を w \in M に変更
      ・区間畳み込み： d_l * d_{l+1} * ... * d_{r - 1}

    Def. モノイド（monoid）
      集合 M と M 上の２項演算子 * の組 (M, *) が以下の条件を満たすときモノイドと呼ぶ
      　・結合律：任意の x, y, z \in M に対して，(x * y) * z = x * (y * z)
      　・単位元の存在：ある e \in M が存在して，任意の x \in M に対して x * e = e * x = x

      結合律だけをみたすものを半群（semigroup）と呼ぶ（半群に単位元を含めたものがモノイド）．
      半群はモノイドにすることができる（半群に含まれない要素を単位元となるように加える）．
      モノイドに逆元の存在を加えたものを群（group)と呼ぶ．
      　・逆元の存在：任意の x \in M に対して，ある y \in M が存在して x * y = y * x = e
      交換律を満たすな群を可換群（commutative group）またはアーベル群（abelian group）と呼ぶ．
      　・可換律：任意の x, y \in M に対して x * y = y * x

      e.g. (R, +, 0), (R, max, -inf), (R, min, inf) はモノイド
      　　　（R は実数全体，3番目の要素は単位元）

  # Complexity
    - Time:
      + 点変更: O(log n)
      + 区間畳み込み: O(log n)
      + 要素の取得: O(1)
      + 配列で初期化: O(n)
      + 値で初期化: O(n)
    - Memory: 2^k <= n を満たす最小の k に対して配列のサイズは 2^{k + 1}

  # Usage
    - SegmentTree<monoid<T>> seg(n): 要素数 n の T型の monoid からなる区間木を構築
    - SegmentTree<monoid<T>> seg(first, last): 指定された範囲 [first, last] の要素で区間木を構築
      + min_monoind<T>:
      　　要素の型が T，二項演算子 min，単位元は型Tの最大値とするモノイド（Range Minimum Query）
      + max_monoind<T>:
      　　要素の型が T，二項演算子 max，単位元は型Tの最小値とするモノイド（Range Maximum Query）
      + sum_monoind<T>:
      　　要素の型が T，二項演算子 +，単位元は 0 とするモノイド（Range Sum Query）

    - seg.resize(first, last): 指定された範囲 [first, last] の要素で区間木を再構築
    - seg.fill(val): 列の要素をすべて val に初期化（O(n)時間）
    - seg.update(i, val): d_i に val を代入
    - seg.accumulate(l, r): d_l * d_{l+1} * ... * d_{r - 1} を返す（クエリ区間は[l, r) ）
    - seg[i]: d_i の値を返す
    - seg.initialize(): 現在の列の値で区間木を構築（fill や seg(d) で使用）

  # Description
    モノイド上のサイズ n の列に対して，値の変更と区間に対する二項演算子の畳み込みを O(log n) 時間
    で行うデータ構造．非再帰で実装をしているのであり本（再帰）よりも定数倍高速．
    配列による初期化や，現在の列の値に対して O(n)時間で構築可能である（O(n log n) 時間ではない）．

    実装は @AI.Cash さんを参考にしており，抽象化等は @tomcatowl さんと @tubo28 さんを
    参考にしている．

  # Note
    - 区間畳み込みという用語が適切かどうかは分からない
    　（accumulate という用語 は C++言語の std::accumulate から選択）
    - クエリのインデックスは 0-index だが，内部の実装では 1-index となっている（コンパクトになる）
    - min_monoid，max_monoid や sum_monoid は可換（commutative）だが，
    　非可換の演算に対応するために v の親から値を更新する演算 d[v >> 1] = Monoid::op(d[v], d[p ^ 1])
    　から変更している．

  # References
    - [Efficient and easy segment trees @AI.Cash]
      (https://codeforces.com/blog/entry/18051)
    - [プログラミングコンテストでのデータ構造 @秋葉拓哉]
      (https://www.slideshare.net/iwiwi/ss-3578491)
    - [データ構造と代数（前半）@tomcatowl]
      (https://tomcatowl.github.io/post/ds-and-alg-1/)
    - [セグメントツリー (点変更・区間クエリ) @tubo28]
      (https://tubo28.me/algorithm/segtree_monoids/)
    - [Segment Tree を少し速くする @hiragushi]
      (http://proc-cpuinfo.fixstars.com/2017/07/optimize-segment-tree/)

  # Verified
    - [AOJ DSL_2_A Range Query - Range Minimum Query (RMQ)]
      (http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=DSL_2_A)
    - [AOJ DSL_2_B Range Query - Range Sum Query (RSQ)]
      (http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=DSL_2_B)
*/

#include <iostream>
#include <vector>
#include <limits>

// -------------8<------- start of library -------8<------------------------
template<typename Monoid>
struct SegmentTree {
    using T = typename Monoid::value_type;

    std::size_t sz;
    std::vector<T> d;

    explicit SegmentTree(std::size_t n = 0) {
        for (sz = 1; sz < n; ) sz <<= 1;
        d.resize(2 * sz, Monoid::unit());
    }

    template<typename InputIterator>
    SegmentTree(InputIterator first, InputIterator last) { resize(first, last); }

    template<typename InputIterator>
    void resize(InputIterator first, InputIterator last) {
        std::size_t n = std::distance(first, last);
        for (sz = 1; sz < n; ) sz <<= 1;
        d.resize(2 * sz, Monoid::unit());
        std::copy(first, last, d.begin() + sz);
        initialize();
    }

    void fill(const T &value) {
        std::fill(d.begin() + sz, d.end(), value);
        initialize();
    }

    void initialize() {
        for (int i = sz - 1; 0 < i; --i) d[i] = Monoid::op(d[i << 1], d[i << 1 | 1]);
    }

    void update(std::size_t idx, const T &value) {
        for (d[idx += sz] = value; idx >>= 1; )
            d[idx] = Monoid::op(d[idx << 1], d[idx << 1 | 1]);
    }

    T accumulate(std::size_t l, std::size_t r) const {
        T res_l = Monoid::unit(), res_r = Monoid::unit();
        for (l += sz, r += sz; l < r; l >>= 1, r >>= 1) {
            if (l & 1) res_l = Monoid::op(res_l, d[l++]);
            if (r & 1) res_r = Monoid::op(d[--r], res_r);
        }
        return Monoid::op(res_l, res_r); // for non-commutative
    }

    T operator[](std::size_t idx) const { return d[sz + idx]; }
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

    // AOJ DSL_2_A Range Query - Range Minimum Query (RMQ)
    SegmentTree<min_commutative_monoid<int>> rmq(n);
    for (int i = 0, com, x, y; i < q; ++i) {
        std::cin >> com >> x >> y;
        if (com == 0) rmq.update(x, y);
        else std::cout << rmq.accumulate(x, y + 1) << '\n';
    }

    // AOJ DSL_2_B Range Query - Range Sum Query (RSQ)
    SegmentTree<sum_commutative_group<int>> rsq(n);
    for (int i = 0, com, x, y; i < q; ++i) {
        std::cin >> com >> x >> y;
        if (com == 0) rsq.update(x - 1, rsq[x - 1] + y);
        else std::cout << rsq.accumulate(x - 1, y) << '\n';
    }

    return 0;
}
