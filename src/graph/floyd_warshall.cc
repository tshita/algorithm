/*
  All Pairs Shortest Path Problem (Floyd-Warshall Algorithm)
  全点対最短路問題（フロイド・ワーシャル法）
  ==============================================================

  # Problem
    Input: 有向グラフ G = (V, E), 辺重み w : E -> R, 始点 s
    Output: 任意の2頂点 u, v 間の最短距離 d[u][v]

  # Complexity (n = |V|, m = |E|)
    Time: O(n^3)
    Memory: O(n^2)

  # Usage
    - Graph<T> g(n, s): 辺重みの型T，頂点数 n
    - g.add_arc(u, v, w): 重み w の弧 (u, v) を追加
    - g.add_edge(u, v, w): 重み w の辺 {u, v} を追加
    - g.FloydWarshall(): フロイド・ワーシャル法で最短距離を求める
    - g.distance(u, v): u から v への最短距離を返す. g.INF と等しい場合は最短距離は存在しない
    - g.CheckNegativeCycle(): g に負閉路が存在するか判定する (\exist v \in V, d[v][v] < 0)

  # Description
    - 頂点部分集合 [0, k] \cup {i, j} を使って i から j への最短距離 d[i][j] を求めるのは，
      頂点部分集合 [0, k) \cup {i, j} での最短距離か，追加された頂点 k を通る d[i][k] + d[k][j]
      のどちらかであるということを用いて動的計画法で解いている
    - d[v][v] < 0 となる頂点 v が存在する場合は負閉路が存在する
      （初め d[v][v] = 0 なので，ある k で d[v][k] + d[k][v] の結果が負となる．
        これは，v から k　への経路と k から v への経路を併せたもので負閉路である．
        逆に，負閉路が存在するときには漸化式の定義から d[v][v] < 0 となる）

  # References
    - あり本 pp. 98--99

  # Verified
    - [AOJ Shortest Path - All Pairs Shortest Path]
      (http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=GRL_1_C )
*/

#include <iostream>
#include <vector>
#include <limits>

// -------------8<------- start of library -------8<------------------------
template<class T>
struct Graph {
    const T INF = std::numeric_limits<T>::max();
    const int n;
    bool is_neg_cycle;
    std::vector<std::vector<T>> d;

    explicit Graph(int _n) :
        n(_n), is_neg_cycle(false), d(n, std::vector<T>(n, INF)) { }

    void add_arc(int src, int dst, T w) { d[src][dst] = w; }
    void add_edge(int src, int dst, T w) { d[src][dst] = d[dst][src] = w; }
    T distance(const int src, const int dst) const { return d[src][dst]; }
    bool CheckNegativeCycle() const { return is_neg_cycle; }

    void FloydWarshall() {
        for (int v = 0; v < n; ++v) d[v][v] = 0;
        for (int k = 0; k < n; ++k)
            for (int i = 0; i < n; ++i) {
                if (INF <= d[i][k]) continue;
                for (int j = 0; j < n; ++j) {
                    if (INF <= d[k][j]) continue;
                    d[i][j] = std::min(d[i][j], d[i][k] + d[k][j]);
                }
            }
        for (int v = 0; v < n; ++v)
            if (d[v][v] < 0) is_neg_cycle = true;
    }
};
// -------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    // AOJ GRL_1_C Shortest Path - All Pairs Shortest Path
    int n, m, s, t, d;
    std::cin >> n >> m;

    Graph<int> g(n);
    for (int i = 0; i < m; ++i) {
        std::cin >> s >> t >> d;
        g.add_edge(s, t, d);
    }

    g.FloydWarshall();
    if (g.CheckNegativeCycle()) std::cout << "NEGATIVE CYCLE\n";
    else {
        for (int src = 0; src < n; ++src) {
            for (int dst = 0; dst < n; ++dst) {
                if (g.INF <= g.distance(src, dst)) std::cout << "INF";
                else std::cout << g.distance(src, dst);
                std::cout << " \n"[dst == n -1];
            }
        }
    }

    return 0;
}
