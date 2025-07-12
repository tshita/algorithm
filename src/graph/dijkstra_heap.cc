/*
  Single Source Shortest Path Problem (Dijkstra with heap)
  単一始点全終点最短路問題（ヒープを使用したダイクストラ法）
  ========================================================

  # Problem
    Input: 有向グラフ G = (V, E), 辺重み w : E -> R, 始点 s
    Output: s から各頂点 v への最短距離 d[v]

  # Complexity (n = |V|, m = |E|)
    Time: O((n + m) * log n)
    Memory: O(n + m)

  # Usage
    - Graph<T> g(n, s): 辺重みの型T，頂点数 n, 始点 s のグラフを構築．
                        2点間最短距離で終点 t が決まっている場合は g(n, s, t) とする
    - g.add_arc(u, v, w): 重み w の弧 (u, v) を追加
    - g.Dijkstra(): ダイクストラ法で最短距離を求める（t が定まっている場合はif文をコメントアウト）
    - g.distance(t): s から t への最短距離を返す. 経路が存在しない場合は INF を返す
    - g.ChangeSource(s): 始点を変更して再度ダイクストラ法（TODO: 計算量を減らす方法があるかも）

  # Note
    - 負閉路が存在しない場合は有限回の反復で終了（それ以外の場合はベルマン・フォード法等を使用）
    - 密なグラフは，ヒープを使用しないダイクストラ法 O(n^2) dijkstra.cc を使用する
    - 経路復元が必要な場合は dijkstra_heap_sol.cc を使用
    - s-t最短路問題の場合はDijkstraのifをコメントアウトすると最適値が見つかったときに途中で打ち切る

  # References
    - あり本 pp. 96--97

  # Verified
    - [AOJ Graph2 - Single Source Shortest Path1]
      (http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=ALDS1_12_B)
*/

#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <tuple>

// -------------8<------- start of library -------8<------------------------
template<class T>
struct Graph {
    struct Edge {
        int dst; T w;
        Edge() {};
        Edge(int _d, T _w) : dst(_d), w(_w) {}
    };

    const T INF = std::numeric_limits<T>::max();
    const int n;
    int s, t;
    std::vector<std::vector<Edge>> adj;
    std::vector<T> dist;

    Graph(int _n, int _s, int _t = -1)
        : n(_n), s(_s), t(_t), adj(n), dist(n, INF) { }

    void add_arc(int u, int v, T w) { adj[u].emplace_back(Edge(v, w)); }
    T distance(const int _t) const { return dist[_t]; }
    void ChangeSource(const int _s) {
        s = _s; std::fill(dist.begin(), dist.end(), INF); Dijkstra();
    }

    void Dijkstra() {
        using State = std::pair<T, int>;
        std::priority_queue<State, std::vector<State>, std::greater<State>> que;

        que.emplace(0, s);
        dist[s] = 0;

        while (!que.empty()) {
            T d; int v;
            std::tie(d, v) = que.top(); que.pop();

            if (dist[v] < d) continue;
            // if (v == t) return ;
            for (const auto &e : adj[v]) {
                if (d + e.w < dist[e.dst]) {
                    dist[e.dst] = d + e.w;
                    que.emplace(dist[e.dst], e.dst);
                }
            }
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
