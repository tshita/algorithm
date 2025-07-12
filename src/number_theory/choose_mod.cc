/*
  Binomial coefficient modulo prime (naive method)
  素数を法とした二項係数（愚直な方法）
  =================================

  # Description
    素数 m を法とした，階乗・階乗の逆元・k-階乗・二項係数・多重集合係数を愚直に求める

    1. Factorial(n) : O(n)時間
       nの階乗(factorial) n!
    2. InvFactorial(n) : O(n + log m)時間
       nの階乗の逆元 (n!)^-1
    3. Permutation(n, k) : O(k)時間
       k-permuation of n P(n, k) = n * (n - 1) * ... * (n - k + 1)
    4. Choose(n, k) : O(n / 2 + log m)時間
       二項係数(binomial coefficeint) \binom{n}{k} = n! / k! * (n-k)!
    5. MultiChoose(n, k) : O(n + k - 1 / 2 + log m)時間
       多重集合係数(multiset coefficient) = \binom{n + k - 1}{k}

  # Note
    - ./modular_arithmetics.cc をインクルードする

  # References
    - [@保坂和宏 プログラミングコンテストでの数え上げ&整数論テクニック]
      (http://hos.ac/slides/20130319_enumeration.pdf)

  # Verified
    - [yukicoder No.117 組み合わせの数](https://yukicoder.me/problems/no/117)

*/

#include <iostream>
#include <vector>

//  @require ./modular_arithmetics.cc 👇👇

// -------------8<------- start of library -------8<------------------------
ModInt Factorial(int n) {
    ModInt fact = n;
    while (--n) fact *= n;
    return fact;
}
inline ModInt InvFactorial(int n) { return Factorial(n).inv(); }

ModInt Permutation(int n, int k) {
    if (k < 0 || n < k) return ModInt(0);
    ModInt p = 1;
    for (int i = 1; i <= k; ++i) p *= n--;
    return p;
}
ModInt Choose(int n, int k) {
    if (n < 0 || k < 0 || n < k) return ModInt(0);
    k = std::min(k, n - k);
    ModInt p = 1, q = 1;
    for (int i = 1; i <= k; ++i) {
        p *= n--;
        q *= i;
    }
    return p / q;
}
ModInt MultiChoose(const int n, const int k) {
    if (n < 0 || k < 0) return ModInt(0);
    else return k == 0 ? 1 : Choose(n + k - 1, k);
}
// -------------8<------- end of library ---------8-------------------------

int main() {
    int T, n, k;
    char c;
    scanf("%d\n", &T);

    while (T--) {
        scanf("%c(%d,%d)\n", &c, &n, &k);
        if (c == 'C') printf("%d\n", Choose(n, k).v);
        else if (c == 'P') printf("%d\n", Permutation(n, k).v);
        else if (c == 'H') printf("%d\n", MultiChoose(n, k).v);
    }

    return 0;
}
