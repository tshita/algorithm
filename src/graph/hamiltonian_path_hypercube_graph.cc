/*
  Hamiltonian Path Problem In Hypercube Graph
  超立方体グラフのハミルトン道問題
  ====================================

  # Problem
    Input: 非負整数 n, s, t (1 <= n <= 32, 0 <= s, t < 2^n)
    Output: n 次元 hypercube graph 上の頂点 s から t へのハミルトン道

    Def. n 次元 Hypercube Graph
      n 次元 Hypercube graph とは無向グラフ G = (V, E) で，
        - 頂点集合 V = {0, 1, ..., 2^n - 1}
        - 辺集合 E = {uv | u と v の二進数表現でちょうど1つのビットが異なる}
      を満たすもの．

    Def. ハミルトン道
      無向グラフ G = (V, E) と頂点 s, t \ in V に対して，s-t ハミルトン道とは，
      s から t へのすべての G の頂点を通る単純道

  # Complexity
    - Time: O(n 2^n)

  # Usage
    - HypercubeGraph::IsHamiltonianPath(n, s, t):
        n 次元 hypercube graph 上で s-t ハミルトン道が存在するか
    - HypercubeGraph::PrintHamiltonianPath(n, s, t):
        n 次元 hypercube graph 上の s-t ハミルトン道を標準出力に出力

  # Description
    n 次元 hypercube graph H = (V, E) 上の s-t ハミルトン道が存在するための必要十分条件は，
    s と t を２進数表現したときの1が立っているビット数のパリティが異なることである．

    必要条件は，|V| = 2^n かつ隣接する頂点対のビット数がちょうど1つ異なることから分かる．
    十分条件は n に関する帰納法で証明できる．s と t の異なるビットが少なくとも1つ存在して，それが x ビット目だとする．
    y を x とは異なる 1 以上 n 以下の整数とする．また，m1 を s の y ビット目だけが反転している整数として，
    m2 を s の x ビット目と y ビット目だけが反転する整数とする．このとき，s から m1 へ x ビット目が s と同じに固定された
    n - 1 次元 hypercube graph 上のハミルトン道 P1 が存在する．また，m2 から t へ x ビット目が t と同じに固定された
    n - 1 次元 hypercube graph 上のハミルトン道 P2 が存在する．また，m1 と m2 は隣接しているので，
    道 s P1 m1 m2 P2 t は s-t ハミルトン道となる．
    この構成方法を再帰的に実装したのが PrintHamiltonianPath() である．

  # Verified
    - [AtCoder Grand Contest 031 C - Differ by 1 Bit]
      (https://atcoder.jp/contests/agc031/tasks/agc031_c)
*/

#include <cstdio>

// -------------8<------- start of library -------8<------------------------
namespace HypercubeGraph {
    bool IsHamiltonianPath(const int n, const int s, const int t) {
        bool has_sol = (__builtin_popcount(s) % 2) != (__builtin_popcount(t) % 2);
        if ((s < (1LL << n)) && (t < (1LL << n)) && has_sol) return true;
        else return false;
    }

    void FindHamiltonianPath(const int s, const int t, const int mask) {
        if (__builtin_popcount(mask) == 1) {
            printf("%d\n%d\n", s, t);
            return ;
        }

        int diff = 1, add = 1;
        while (!(diff & mask) || !((s ^ t) & diff)) diff <<= 1;
        while (!(diff != add) || !(add & mask)) add <<= 1;

        FindHamiltonianPath(s, s ^ add, mask ^ diff);
        FindHamiltonianPath(s ^ diff ^ add, t, mask ^ diff);
    }

    void PrintHamiltonianPath(const int n, const int s, const int t) {
        if (IsHamiltonianPath(n, s, t)) FindHamiltonianPath(s, t, (1 << n) - 1);
    }
}
// -------------8<------- end of library ---------8-------------------------

int main() {
    int N, A, B;
    scanf("%d %d %d", &N, &A, &B);

    if (HypercubeGraph::IsHamiltonianPath(N, A, B)) {
        puts("YES");
        HypercubeGraph::PrintHamiltonianPath(N, A, B);
    } else {
        puts("NO");
    }

    return 0;
}
