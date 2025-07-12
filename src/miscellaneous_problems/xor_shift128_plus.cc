/*
  XorShift128 plus (David Blackman, Sebastiano Vigna in 2016)
  ===========================================================

  # Problem
    Input: 疑似乱数のseed
    Output: 周期 2^128 - 1 の疑似乱数

  # Usage
    - XorShift128 xs(seed): seed を設定 (seed に splitmix64.c を使用して新たに設定)
    - xs.min(): xs の最小値
    - xs.max(): xs の最大値
    - xs(): 次の疑似乱数を出力. xs.next() と同じ．
    - xs.range(l, r): [l, r] の範囲の疑似乱数を生成

  # Description
    疑似乱数 XorShift128 plus の Blackman, Vigna の実装をクラス化．
    高速な疑似乱数生成器で周期も 2^128 - 1 と長い．

  # Note
    - xs.jump() と xs.long_jump() は実装していない (必要な場合は xoroshiro128plus を参照)
    - C++ <random> のパラメータ定義済み疑似乱数生成器と同様な使い方ができる．
      例えば，分布生成器(ex. 一様分布, ベルヌーイ分布)を使用できる．
    - 整数一様乱数 xs.range() は <random> の uniform_int_distribution よりも10倍高速．
    - 整数一様乱数に剰余を使用する方法は厳密には一様ではないので注意．ただし，高速．

  # References
    - [xoroshiro128plus.c](http://xoroshiro.di.unimi.it/xoroshiro128plus.c)
    - [splitmix64.c](http://xoroshiro.di.unimi.it/splitmix64.c)
*/

#include <iostream>
#include <random>

using namespace std;

// -------------8<------- start of library -------8<------------------------
class XorShift128 {
public:
    using result_type = uint_fast64_t;
    static constexpr result_type min() { return 0u; }
    static constexpr result_type max() { return UINT_FAST64_MAX; }
    result_type operator() () { return next(); }

    explicit XorShift128(result_type seed = 0) : s{0, 0} { init(seed); }

    // 整数の場合に uniform_int_distribution を使うと10倍ぐらい遅い
    result_type range(const result_type lb, const result_type ub) {
        if (ub == max()) return (*this)();
        const result_type limit = (max() / (ub - lb + 1)) * (ub - lb + 1);
        result_type r;
        while (limit <= (r = (*this)()));
        return (r % (ub - lb + 1)) + lb;
    }

private:
    result_type s[2];

    // using splitmix64 for initialization
    void init(const result_type seed) {
        x = seed;
        for (int i = 0; i < 100 || s[0] == 0 || s[1] == 0; ++i) {
            s[0] = splitmix64_next(); s[1] = splitmix64_next();
        }
    }

    static inline result_type rotl(const result_type x, int k) {
        return (x << k) | (x >> (64 - k));
    }

    result_type next() {
        const result_type s0 = s[0];
        result_type s1 = s[1];
        const result_type result = s0 + s1;
        s1 ^= s0;
        s[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14); // a, b
        s[1] = rotl(s1, 36); // c
        return result;
    }

    // splitmix64 (http://xoroshiro.di.unimi.it/splitmix64.c)
    result_type x; /* The state can be seeded with any value. */
    result_type splitmix64_next() {
        result_type z = (x += 0x9e3779b97f4a7c15);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
        z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
        return z ^ (z >> 31);
    }
};
// -------------8<------- end of library ---------8-------------------------


int main() {
    XorShift128 xs(0); // seed を 0 に設定

    // (0) xs.min() <= xs() < xs.max()
    std::cout << xs() << std::endl;

    // (1) 範囲整数乱数 [2, 10] : 正確には一様乱数ではないが (2), (3) より高速
    std::cout << xs() % (10 - 2 + 1) + 2 << std::endl;

    // (2) 整数一様乱数 [2, 10]
    std::cout << xs.range(2, 10) << std::endl;

    // (3) 整数一様乱数 [2, 10] , (2)より10倍遅い
    std::uniform_int_distribution<> dist_i(2, 10);
    std::cout << dist_i(xs) << std::endl;

    // (4) 実数一様乱数 [2.5, 10.0]
    std::uniform_real_distribution<> dist_r(2.5, 10.0);
    std::cout << dist_r(xs) << std::endl;

    return 0;
}
