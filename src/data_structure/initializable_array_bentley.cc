/*
  Initializable Array by Bentley
  初期化可能な配列
  ==============================

  # Problem
    配列のすべての要素に同一の値を代入する初期化の操作を O(1) 時間で可能な配列

  # Usage
    - InitializableArray<T> x(n): 型 T の n 個の要素からなる配列 x を構築 [Time O(n)]
    - set(i, a): x[i] に a を代入 [Time O(1)]
    - x[i]: x[i] を返す [Time O(1)]
    - init(a): x のすべての要素に a を代入 [Time O(1)]
    - size(): 配列の要素数を返す [Time O(1)]
    - resize(n): 要素数を n に変更（前の要素は全てクリアされる）[Time O(n)]

  # Description
    init(a) を実現するために領域を (2 * n + 2) * sizeof(size_t) + sizeof(T) だけ追加している．
    上の操作を実現する抽象配列を Z とする．T型変数 initv は最近行われた初期化操作 init(a) の a の値を表す．
    Z は要素数 n の配列 from と スタック to（スタックサイズ b，初期サイズ 0）を用いたチェインという概念で実現する．
    Z[i] は i 番目がチェインのとき value[i] で，チェインでないとき initv とする．
    すなわち，i 番目がチェインであるとは，i 番目の要素が直近の初期化後に代入されたかどうかを表す．

    i 番目がチェインであるとは，値 i がスタック to に push されており，from[i] と to がリンクしていることである．
    from[i] と to がリンクしているとは，値 i が to の from[i] 番目に push されていることを意味する．
    すなわち，to[from[i]] = i を満たすことである．また，i が to に push されていることから
    to のスタックサイズ b に対しても from[i] < b を満たす．

    init(a) はスタックを空にして（b = 0），initv = a とすればよい．
    set(i, a) は i 番目がチェインのときは value[i] = a として，
    チェインでないときはスタックに i を追加して，from[i] と to をリンクしてから value[i] = a とする．
    x[i] も i　番目がチェインであるかどうかで value[i] を返すか initv を返すかを行えばよい．
    これらは，すべて O(1) 時間で実現可能である．

  # Note
    配列外参照のチェックは行っていない（必要なら入れる）

    @tmaehara さんの実装を vector にしただけ・・・．TODO としてどっちが高速なのかの実験を行う．
    @kgoto さんのブログに追加領域をより少なくできる論文があると書かれているのでいつか実装する．

    アルゴリズムイントロダクションの演習問題11.1-4にある，直接アドレス表が巨大な配列のときに探索，挿入，削除を
    O(1)時間で行えという問題に対して，直接アドレス表をこのデータ構造にすることで実現できる．

  # References
    - [spagetthi source @tmaehara]
      (https://github.com/spaghetti-source/algorithm/blob/master/data_structure/initializable_array.cc)
    - [初期化配列の実装 @kgoto](https://qiita.com/kgoto/items/0251e442292d8ebc1f3d)

  # Verified

*/

#include <iostream>
#include <vector>

// -------------8<------- start of library -------8<------------------------
template <class T>
class InitializableArray {
    T initv;
    size_t n = 0, b = 0;
    std::vector<T> value;
    std::vector<size_t> from, to;

    bool chain(const size_t i) const { return from[i] < b && to[from[i]] == i; }

public:
    explicit InitializableArray(int _n) : n(_n), value(n), from(n), to(n) {}

    T operator[](const size_t i) const { return chain(i) ? value[i] : initv; }
    size_t size() const { return n; }
    void resize(const size_t _n) {
        n = _n; b = 0;
        value.resize(n); from.resize(n); to.resize(n);
    }

    void init(const T &a) { initv = a; b = 0; }
    void set(const size_t i, const T &a) {
        if (!chain(i)) { from[i] = b; to[b++] = i; }
        value[i] = a;
    }
};
// -------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    constexpr int n = 5;
    InitializableArray<int> x(n);

    for (int i = 0; i < n; ++i) x.set(i, i);
    for (int i = 0; i < n; ++i) std::cout << x[i] << " \n"[i == n -1];

    x.init(2 * n);
    for (int i = 0; i < n; ++i) std::cout << x[i] << " \n"[i == n -1];

    x.set(0, 0);
    x.set(n - 1, n - 1);
    for (int i = 0; i < n; ++i) std::cout << x[i] << " \n"[i == n -1];

    x.init(3 * n);
    for (int i = 0; i < n; ++i) std::cout << x[i] << " \n"[i == n -1];

    return 0;
}
