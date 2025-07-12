/*
  GCD and LCM
  最大公約数と最小公倍数
  =================

  # Problem
    最大公約数(GCD; greatest common divisor)
      Input: 整数 a, b
      Output: max_{整数 c, a | c and b | c} (c)

    最小公倍数(LCM; least common multiple)
      Input: 整数 a, b
      Output: min_{整数 c, c | a and c | a} (c)

  # Complexity
    Time : O(log(min(a, b)))

  # Algorithm
    整数 a, b (a >= b) の最大公約数 gcd(a, b) はユークリッドの互除法，
     gcd(a, b) = gcd(b, a % b)
    を再帰的に計算している．最小公倍数 lcm(a, b) は，
     ab = gcd(a, b) * lcm(a, b)
    を最大公約数を用いて計算している．

  # Note
    - C++17からSTLに追加. std::gcd(a, b) と std::lcm(a, b)
　　 - GCC拡張に最大公約数 __gcd(a, b) がある

*/

#include <iostream>

// -------------8<------- start of library -------8<------------------------
using ll = long long;

inline ll gcd(ll a, ll b) { return b ? gcd(b, a % b) : a; }
inline ll lcm(ll a, ll b) { return a / gcd(a, b) * b; }
// -------------8<------- end of library ---------8-------------------------

int main() {
    ll a = 8, b = 12;
    std::cout << gcd(a, b) << std::endl; // 4
    std::cout << lcm(a, b) << std::endl; // 24

    return 0;
}
