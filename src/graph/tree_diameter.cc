/*
  Diameter of a tree
  木の直径
  ==================

  # Problem
    Input: 木 T = (V, E), 辺重み c : E -> R
    Output: T の直径

    無向グラフ G = (V, E), 任意の u, v \in V の最短距離を d(u, v) とする.
    Def. v の離心数 (eccentricity)
      頂点 v から最も遠い頂点までの距離を v の離心数と呼び e(v) で表す．
        e(v) = max_{u \in V} d(v, u)

    Def. G の直径 (diameter)
      G の2頂点間の最大距離を G の直径と呼び diam(G) で表す．
        diam(G) = max_{v \in V} e(v)

  # Complexity (n = |V|)
    - Time : O(n)

  # Usage
    - Tree<T> tree(n): 辺重みの型 T, 頂点数 n の木を構築
    - tree.add_edge(u, v, w): 重み w の辺 {u, v} を追加
    - tree.Diameter(): tree の直径を返す
    - tree.farthest_pair: (first, second) は最遠頂点対

  # Description
    任意の頂点 s からの最遠頂点を u として，u からの最遠頂点を v としたとき，u と v が最遠頂点対となる．

    Thm. T の直径は d(u, v)
      頂点 x, y を最遠頂点対とする．s を根とする根付き木を考える．x と u の最小共通祖先を t とする．
      (1) t の部分木に y が含まれる場合
        u が s の最遠頂点なので d(t, y) <= d(t, u) となり，d(x, y) <= d(x, u) <= d(v, u)
      (2) それ以外
        u が s の最遠頂点なので d(y, x) <= d(y, u) <= d(v, u)

  # References
    - [Spagetthi Source](http://www.prefield.com/algorithm/graph/tree_diameter.html)
    - [電子情報通信学会『知識の森』12群2編3章 p.6]
      (http://www.ieice-hbkb.org/files/12/12gun_02hen_03.pdf)

  # Verified
    - [AOJ GRL_5_A Tree - Diameter of a Tree]
      (http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=GRL_5_A)
    - [AOJ1196 Brdige Removal]
      (http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=1196)

*/

#include <iostream>
#include <vector>

// -------------8<------- start of library -------8<------------------------
template<class W>
struct Tree {
    int n;
    std::vector<std::vector<std::pair<int, W>>> adj;

    explicit Tree(int _n) : n(_n), adj(_n) {}
    void add_edge(const int v1, const int v2, const W w) {
        adj[v1].emplace_back(std::make_pair(v2, w));
        adj[v2].emplace_back(std::make_pair(v1, w));
    }

    std::pair<int, W> Dfs(const int prev, const int cur) {
        std::pair<int, W> res(cur, 0);
        for (const auto &e : adj[cur]) {
            if (prev == e.first) continue;
            auto nxt = Dfs(cur, e.first); nxt.second += e.second;
            if (res.second < nxt.second) res = nxt;
        }
        return res;
    }

    std::pair<int, int> farthest_pair;
    W Diameter() {
        const auto v1 = Dfs(-1, 0), v2 = Dfs(-1, v1.first);
        farthest_pair = std::make_pair(v1.first, v2.first);
        return v2.second;
    }
};
// -------------8<------- end of library ---------8-------------------------


int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    // AOJ GRL_5_A Tree - Diameter of a Tree
    int n;
    std::cin >> n;

    Tree<int> tree(n);
    for (int i = 0, s, t, w; i < n - 1; ++i) {
        std::cin >> s >> t >> w;
        tree.add_edge(s, t, w);
    }

    std::cout << tree.Diameter() << std::endl;

    return 0;
}
