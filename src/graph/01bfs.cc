/*
  Single Source Shortest Paths Problem in a Binary Weighted Digraph (0-1 BFS)
  0-1 重み付きグラフの単一始点全終点最短路問題（0-1 BFS）
  ==========================================================================

  # Problem
    Input: 有向グラフ G = (V, E), 弧重み w : E -> {0, 1}, 始点 s
    Output: s から各頂点 v への最短距離 d[v]

  # Complexity (n = |V|, m = |E|)
    Time: O(n + m)
    Memory: O(n + m)

  # Usage
    - Graph<T> g(n, s): 弧重みの型T，頂点数 n, 始点 s のグラフを構築．
                        2点間最短距離で終点 t が決まっている場合は g(n, s, t) とする
    - g.add_edge(u, v, w): 重み w の弧 (u, v) を追加
    - g.BinaryBfs(): 0-1 BFS を行う
    - g.distance(t): s から t への最短距離を返す. 経路が存在しない場合は INF を返す

  # Description
    弧重みが 1 だけの最短路問題は幅優先探索(Breadth First Search; BFS)を用いて O(n + m)時間で
    解くことができるが，弧重みに 0 が含まれると工夫が必要である．
    弧重みが 0, 1 の最短路問題は double-ended queue (二重終端キュー; デック) を用いる．
    デックとは先頭と末尾の要素へのアクセス・削除・追加が O(1)時間で可能なデータ構造である．
    基本的には幅優先探索を行うが，重み 0 の弧はデックの先頭に追加して，重み 1 の弧はデックの末尾に追加する．

  # References
    - [保坂和宏，グラフ探索アルゴリズムとその応用]
      (http://hos.ac/slides/20110504_graph.pdf#page=50)

  # Verified
    - [AtCoder Beginner Contest 077 D - Small Multple]
      (https://beta.atcoder.jp/contests/abc077/tasks/arc084_b)
*/

#include <iostream>
#include <vector>
#include <limits>
#include <deque>

// -------------8<------- start of library -------8<------------------------
template<class T>
struct Graph {
    const T INF = std::numeric_limits<T>::max();
    const int n;
    int s, t;
    std::vector<std::vector<std::pair<int, int>>> adj;
    std::vector<T> d;

    Graph(int _n, int _s, int _t = -1)
        : n(_n), s(_s), t(_t), adj(n), d(n, INF) { }

    void add_edge(int u, int v, T w) { adj[u].emplace_back(std::make_pair(v, w)); }
    T distance(const int _t) const { return d[_t]; }

    void BinaryBfs() {
        std::deque<int> deq;
        d[s] = 0; deq.push_front(s);

        while (!deq.empty()) {
            int cur = deq.front(); deq.pop_front();
            if (cur == t) break;
            for (auto e : adj[cur]) {
                if (d[cur] + e.second < d[e.first]) {
                    d[e.first] = d[cur] + e.second;
                    if (e.second == 0) deq.push_front(e.first);
                    else deq.push_back(e.first);
                }
            }
        }
    }
};
// -------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    constexpr int s = 0;
    int n, m, u, v, w;
    std::cin >> n >> m;

    Graph<int> g(n, s);
    for (int i = 0; i < m; ++i) {
        std::cin >> u >> v >> w;
        g.add_edge(u, v, w);
    }

    g.BinaryBfs();
    for (int v = 0; v < n; ++v)
        std::cout << v << ' ' << g.distance(v) << '\n';

    return 0;
}
