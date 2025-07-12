/*
  Single Source Shortest Path Problem (Dijkstra with heap)
  単一始点全終点最短路問題（ヒープを使用したダイクストラ法）
  ========================================================

  # Problem
    Input: 有向グラフ G = (V, E), 辺重み w : E -> R, 始点 s
    Output: s から各頂点 v への最短距離 d[v] と最短路

  # Complexity (n = |V|, m = |E|)
    Time:  Dijkstra  O((n + m) * log n)
          Find Path  O(n) (after dijkstra)
    Memory: O(n + m)

  # Usage
    - Graph<T> g(n, s): 辺重みの型T，頂点数 n, 始点 s のグラフを構築．
                        2点間最短距離で終点 t が決まっている場合は g(n, s, t) とする
    - g.add_arc(u, v, w): 重み w の弧 (u, v) を追加
    - g.Dijkstra(): ダイクストラ法で最短距離を求める（t が定まっている場合はif文をコメントアウト）
    - g.distance(t): s から t への最短距離を返す. 経路が存在しない場合は INF を返す
    - g.ShortestPath(t): s から t への最短路を返す

  # Note
    - 負閉路が存在しない場合は有限回の反復で終了（それ以外の場合はベルマン・フォード法等を使用）
    - 密なグラフは，ヒープを使用しないダイクストラ法 O(n^2) dijkstra.cc を使用する
    - 経路復元が必要無い場合は dijkstra_heap.cc を使用
    - s-t最短路問題の場合はDijkstraのifをコメントアウトすると最適値が見つかったときに途中で打ち切る

  # References
    - あり本 pp. 96--97

  # Verified

*/

#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <tuple>
#include <algorithm>

// -------------8<------- start of library -------8<------------------------
template<class T>
struct Graph {
    struct Edge {
        int dst; T w;
        Edge() {};
        Edge(int _d, T _w) : dst(_d), w(_w) {}
    };

    const T INF = std::numeric_limits<T>::max();
    const int n, s, t;
    std::vector<std::vector<Edge>> adj;
    std::vector<T> dist;
    std::vector<int> prev;

    Graph(int _n, int _s, int _t = -1)
        : n(_n), s(_s), t(_t), adj(n), dist(n, INF), prev(n, -1) { }

    void add_arc(int u, int v, T w) { adj[u].emplace_back(Edge(v, w)); }
    T distance(const int _t) const { return dist[_t]; }

    std::vector<int> ShortestPath(int _t) {
        if (INF <= dist[_t]) return {};
        std::vector<int> path;
        for ( ; _t != -1; _t = prev[_t]) path.push_back(_t);
        reverse(path.begin(), path.end());
        return path;
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
                    prev[e.dst] = v;
                    que.emplace(dist[e.dst], e.dst);
                }
            }
        }
    }
};
// -------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    int n, m, s, t;
    std::cin >> n >> m >> s >> t;

    Graph<int> g(n, s);
    for (int i = 0; i < m; ++i) {
        int v[2], w;
        std::cin >> v[0] >> v[1] >> w;
        g.add_arc(v[0], v[1], w);
    }

    g.Dijkstra();
    auto path = g.ShortestPath(t);
    for (auto v : path) std::cout << v << '\n';

    return 0;
}
