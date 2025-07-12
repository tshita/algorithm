/*
  Single Source Shortest Path Problem (Dijkstra)
  単一始点全終点最短路問題（ダイクストラ法）
  ========================================================

  # Problem
    Input: 有向グラフ G = (V, E), 辺重み w : E -> R, 始点 s
    Output: s から各頂点 v への最短距離 d[v]

  # Complexity (n = |V|, m = |E|)
    Time: O(n^2)
    Memory: O(n^2)

  # Usage
    - Graph<T> g(n, s): 辺重みの型T，頂点数 n, 始点 s のグラフを構築．
                        2点間最短距離で終点 t が決まっている場合は g(n, s, t) とする
    - g.add_arc(u, v, w): 重み w の弧 (u, v) を追加
    - g.Dijkstra(): ダイクストラ法で最短距離を求める（t が定まっている場合はif文をコメントアウト）
    - g.distance(t): s から t への最短距離を返す. 経路が存在しない場合は INF を返す

  # Note
    - 負閉路が存在しない場合は有限回の反復で終了（それ以外の場合はベルマン・フォード法等を使用）
    - 疎なグラフは，ヒープを使用するダイクストラ法 O((n + m) * log n) dijkstra_heap.cc を使用する
    - s-t最短路問題の場合はDijkstraのifをコメントアウトすると最適値が見つかったときに途中で打ち切る

  # References
    - あり本 pp. 96--97

  # Verified
    - [AOJ Graph2 - Single Source Shortest Path1]
      (http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=ALDS1_12_B )
*/

#include <iostream>
#include <vector>
#include <limits>

// -------------8<------- start of library -------8<------------------------
template<class T>
struct Graph {
    const T INF = std::numeric_limits<T>::max();
    const int n;
    int s, t;
    std::vector<std::vector<T>> adj;
    std::vector<T> d;

    Graph(int _n, int _s, int _t = -1)
        : n(_n), s(_s), t(_t), adj(n, std::vector<T>(n, INF)), d(n, INF) { }

    void add_arc(int u, int v, T w) { adj[u][v] = w; }
    T distance(const int _t) const { return d[_t]; }

    void Dijkstra() {
        std::vector<bool> used(n, false);
        d[s] = 0;
        while (true) {
            int v = -1;
            for (int u = 0; u < n; ++u)
                if (!used[u] && (v == -1 || d[u] < d[v])) v = u;
            if (v == -1 || INF <= d[v]) break;
            // if (v == -1 || INF <= dist[v] || v == t) break;
            used[v] = true;
            for (int u = 0; u < n; ++u)
                if (adj[v][u] != INF) d[u] = std::min(d[u], d[v] + adj[v][u]);
        }
    }
};
// -------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    // AOJ Graph2 - Single Source Shortest Path1
    constexpr int s = 0;
    int n, deg, u, w;
    std::cin >> n;

    Graph<int> g(n, s);
    for (int v = 0; v < n; ++v) {
        std::cin >> deg >> deg;
        while (deg--) {
            std::cin >> u >> w;
            g.add_arc(v, u, w);
        }
    }

    g.Dijkstra();
    for (int v = 0; v < n; ++v)
        std::cout << v << ' ' << g.distance(v) << '\n';

    return 0;
}
