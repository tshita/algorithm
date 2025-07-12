/*
  Extended Euclid's Algorithm
  拡張ユークリッドの互除法
  ==============================================

  # Problem
    Input: 整数 a, b
    Output: ax + by = gcd(a, b) を満たす整数 x, y

  # Complexity
    Time : O(\log\min(a, b))

  # Description
    TODO : 説明を書く

  # References
    - [tubo28 libalgo/拡張ユークリッドの互除法](http://tubo28.me/algorithm/extgcd/)

  # Verified
    - [AOJ NTL_1_E](http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=NTL_1_E)
*/

#include <bits/stdc++.h> // Replace needed headers for speed up

// -------------8<------- start of library -------8<------------------------
using Int = long long;

Int ExtGcd(Int a, Int b, Int &x, Int &y) {
    Int g = a; x = 1; y = 0;
    if (b != 0) { g = ExtGcd(b, a % b, y, x); y-= (a / b) * x; }
    return g;
}
// -------------8<------- end of library ---------8-------------------------

int main() {
    Int a, b, x, y;
    std::cin >> a >> b;

    ExtGcd(a, b, x, y);
    std::cout << x << ' ' << y << '\n';

    return 0;
}
