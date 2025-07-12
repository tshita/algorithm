/*
  Generate primes at compile time
  コンパイル時に素数表を生成
  ===============================

  # Problem
    Sieve of Eratosthenes
    Input: integer N
    Output: i <= N is prime number? (boolean array)

  # Complexity
    Time: O(N log log N)
    Space: O(N)

  # Note
    limitation of N is about 262140 in gcc version 7.3.0
    (constexpr loop iteration count must be less than 262144)
  # Verified
    comprision with wikipedia ``List of prime numbers''
*/

#include <iostream>

// -------------8<------- start of library -------8<------------------------
template<int N>
struct Prime {
    bool is_p[N + 1]; // 素数表

    explicit constexpr Prime() : is_p() {
        for (int i = 2; i <= N; ++i) is_p[i] = true;
        is_p[0] = is_p[1] = false;
        for (long long i = 2; i <= N; ++i) {
            if (is_p[i])
                for (long long j = i * i; j <= N; j += i)
                    is_p[j] = false;
        }
    }
    constexpr bool operator[](const int idx) const { return is_p[idx]; }
};
// -------------8<------- end of library ---------8-------------------------

int main() {
    constexpr int n = 262140;
    constexpr Prime<n> p;

    // Primeクラスがコンパイル時定数ならばエラーを出力
    static_assert(p[5] == false, "5 is prime number");

    for (int i = 0; i <= n; ++i)
        if (p[i]) std::cout << i << std::endl;

    return 0;
}
