#include <iostream>
#include <vector>

using namespace std;

// --------------------8<------- start of library -------8<--------------------
struct Timer {
public:
    Timer() { reset(); }
    void reset() { start = get_time(); }
    double elapsed() { return get_time() - start; }
    bool keep() { return elapsed() < cutoff_time; }

private:
    constexpr static double ticks_per_sec = 2800000000; // 環境によって変える
    constexpr static double ticks_per_sec_inv = 1.0 / ticks_per_sec;
    constexpr static double cutoff_time = 10.0; // 制限時間
    double start;

    inline double get_time() {
        uint32_t lo, hi;
        __asm__ volatile ("rdtsc" : "=a" (lo), "=d" (hi));
        return (((uint64_t)hi << 32) | lo) * ticks_per_sec_inv;
    }
};

// Written in 2016 by David Blackman and Sebastiano Vigna
// XorShift128 plus : period 2^128 - 1 ( http://xoroshiro.di.unimi.it/xoroshiro128plus.c )
class XorShift128 {
public:
    using result_type = uint_fast64_t;
    static constexpr result_type min() { return 0u; }
    static constexpr result_type max() { return UINT_FAST64_MAX; }
    result_type operator() () { return next(); }

    XorShift128(result_type seed = 0) : s{0, 0} { init(seed); }

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
    static inline result_type rotl(const result_type x, int k) { return (x << k) | (x >> (64 - k)); }
    result_type next() {
        const result_type s0 = s[0];
        result_type s1 = s[1];
        const result_type result = s0 + s1;
        s1 ^= s0;
        s[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14); // a, b
        s[1] = rotl(s1, 36); // c
        return result;
    }

    void jump(void) {
        static const result_type JUMP[] = { 0xbeac0467eba5facb, 0xd86b048b86aa9922 };
        result_type s0 = 0, s1 = 0;
        for(size_t i = 0; i < sizeof(JUMP) / sizeof(*JUMP); i++)
            for(int b = 0; b < 64; b++) {
                if (JUMP[i] & UINT64_C(1) << b) { s0 ^= s[0]; s1 ^= s[1]; }
                next();
            }
        s[0] = s0; s[1] = s1;
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
// ---------------------8<------- end of library -------8<---------------------


int main() {
    cin.tie(0); ios::sync_with_stdio(false);

    return 0;
}
