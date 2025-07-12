/*
  RmQ and RMQ : Sparse Table
  ==========================================================

  # Problem
    Input: 冪等モノイド (M, *) 上の列 (d_0, d_1, ..., d_{n-1})
    Query: 区間畳み込み： d_l * d_{l+1} * ... * d_{r - 1}

    Def. モノイド（monoid）
      集合 M と M 上の２項演算子 * の組 (M, *) が以下の条件を満たすときモノイドと呼ぶ
      　・結合律：任意の x, y, z \in M に対して，(x * y) * z = x * (y * z)
      　・単位元の存在：ある e \in M が存在して，任意の x \in M に対して x * e = e * x = x

      モノイド (M, *) が冪等性(idempotence) を満たすとは，任意の x \in M に対して，x * x = x
      が成り立つことである．冪等性を満たすモノイドを冪等モノイド（Idempotent Monoid）と呼ぶ

      e.g. (R, max, -inf), (R, min, inf) は冪等モノイド（(R, +, 0) はモノイドだが冪等性は満たさない）

  # Complexity
    - Time:
      + テーブルの構築: O(n log n)
      + 区間畳み込み: O(1)
    - Memory: O(n \log n)

  # Usage
    - SparseTable<monoid<T>> rmq(n): 要素数 n の T型の冪等モノイドからなる sparse Table を構築
    - SparseTable<monoid<T>> rmq(first, last): 指定された範囲 [first, last] の要素で sparse Table を構築
      + min_monoind<T>:
      　　要素の型が T，二項演算子 min，単位元は型Tの最大値とするモノイド（Range Minimum Query; RmQ）
      + max_monoind<T>:
      　　要素の型が T，二項演算子 max，単位元は型Tの最小値とするモノイド（Range Maximum Query; RMQ）

    - T rmq.accumulate(l, r): d_l * d_{l + 1} * ... * d_{r - 1} を返す（クエリ区間は[l, r)）
    - rmq[i]: d_{i} の値を返す
    - rmq[i] = value: d_{i} に value を代入（変更後に accumulate を使用するときは updateTable() を呼ぶ必要がある）

    - rmq.resize(n): 要素数 n の sparse table を再構築（初期値は単位元）
    - rmq.resize(first, last): 指定された範囲 [first, last] の要素で sparse table を再構築
    - initializeLog2(): 現在の要素数を元に floor(log2(i)) の計算をする（i = 0, ..., floor(log(n)) + 1）
    - updateTable(): sparse table を更新

  # Description
    ここでは RmQ / RMQ を sparse table で求めている．前処理で O(n log n)時間，各クエリに O(1) 時間かかるため，
    主に値の変更がない場合に使用される．

    Sparse Table では二次元の配列を t[p][i] := d[i] * d[i + 1] * ... * d[i + 2^p - 1] として使用する．
    ただし，0 <= p <= floor(log2(n)), 0 <= i < n とする．
    この配列は t[0][i] = d[i] と初期化して，動的計画法で求めている（詳しくは updateTable() を参照）．

    区間クエリ [l, r) は p = floor(log2(r - l)) としたとき，[l, l + 2^p - 1] と
    [r - 2^p, (r - 2^p) + 2^p - 1] の和集合は [l, r) と等しくなるので，
    d[l, .., r - 1] = t[p][l] * t[p][r - 2^p] となる．区間が交差することの証明では
    2^{floor(log2(x))} = floor(x) を使用する．この2つの区間は交差する可能性があるので，冪等性が必要である．
    Sparse Table は O(n^2) 個の区間に対する答えを O(n log n) 領域に圧縮したという見方ができる．

  # Note
    - 主に要素の値が immutable な場合に使用する（値更新後に updateTable を呼び出す必要があり，計算時間は O(n log n)）
    - 値を変更する演算が必要な場合はは区間木などを使用する
    - TODO: O(n) 時間で構築する方法が存在するので気が向いたら実装する

  # References
    - [CP-Algorithms "Sparse Table"](https://cp-algorithms.com/data_structures/sparse-table.html)
    - [@okateim "Range Minimum Query"](https://qiita.com/okateim/items/e2f4a734db4e5f90e410)

  # Verified
    - [SPOJ RMQSQ - Range Minimum Query](https://www.spoj.com/problems/RMQSQ/)
*/

#include <iostream>
#include <vector>
#include <limits>

// -------------8<------- start of library -------8<------------------------
template<typename IdempotentMonoid>
struct SparseTable {
    using T = typename IdempotentMonoid::value_type;

    std::size_t sz = 0;
    std::vector<std::vector<T>> d; // d[p][i] = op(a[i], a[i + 1], ..., a[i + 2^p - 1])
    std::vector<size_t> log2; // log2[i] = floor(log_2(i))


    SparseTable() {}

    explicit SparseTable(std::size_t n) : sz(n) {
        initializeLog2();
        d.resize(log2[sz] + 1, std::vector<T>(sz, IdempotentMonoid::unit()));
        updateTable();
    }

    template<typename InputIterator>
    SparseTable(InputIterator first, InputIterator last) { resize(first, last); }

    void resize(const size_t n) {
        sz = n;
        initializeLog2();
        d.resize(log2[sz] + 1, std::vector<T>(sz, IdempotentMonoid::unit()));
    }

    template<typename InputIterator>
    void resize(InputIterator first, InputIterator last) {
        sz = std::distance(first, last);
        initializeLog2();

        d.resize(log2[sz] + 1, std::vector<T>(sz, IdempotentMonoid::unit()));
        std::copy(first, last, d[0].begin());

        updateTable();
    }

    void initializeLog2() {
        log2.resize(sz + 1); // log2[1] = 0
        for (size_t i = 2; i <= sz; ++i) log2[i] = log2[i >> 1] + 1;
    }

    void updateTable() {
        for (size_t p = 1; p < d.size(); p++)
            for (size_t i = 0; i + (1 << p) <= sz; i++)
                d[p][i] = IdempotentMonoid::op(d[p - 1][i], d[p - 1][i + (1 << (p - 1))]);
    }

    // d[l] * ... * d[r - 1]
    T accumulate(std::size_t l, std::size_t r) const {
        const size_t p = log2[r - l];
        return IdempotentMonoid::op(d[p][l], d[p][r - (1 << p)]);
    }

    T operator[](std::size_t idx) const { return d[0][idx]; }
    T& operator[](std::size_t idx) { return d[0][idx]; }
};

template<typename T>
struct min_monoid {
    using value_type = T;
    static constexpr T unit() { return std::numeric_limits<T>::max(); }
    static T op(const T &lhs, const T &rhs) { return std::min(lhs, rhs); }
};

template<typename T>
struct max_monoid {
    using value_type = T;
    static constexpr T unit() { return std::numeric_limits<T>::min(); }
    static T op(const T &lhs, const T &rhs) { return std::max(lhs, rhs); }
};
// -------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    int n;
    std::cin >> n;

    SparseTable<min_monoid<int>> rmq(n);
    for (int i = 0; i < n; ++i) std::cin >> rmq[i];
    rmq.updateTable();

    int q;
    std::cin >> q;
    for (int i = 0; i < q; ++i) {
        int l, r;
        std::cin >> l >> r;
        std::cout << rmq.accumulate(l, r + 1) << std::endl;
    }

    return 0;
}
