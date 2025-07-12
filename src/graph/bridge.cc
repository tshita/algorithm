/*
  2-edge connected components (Hopcroft and Tarjan)
  2辺連結成分(橋の列挙)
  =================================================

  # Problem
    Input: 無向グラフ G = (V, E)
    Output: G のすべての橋と2辺連結成分

    Def. 橋(bridge)
      G の橋とは e \in E で，G - e の連結成分数が G よりも多くなるもの

    Def. 2辺連結成分(2-edge connected component)
      G の2辺連結成分とは辺部分集合で，そのどの2辺も共通の単純閉路に属し極大なもの
      2辺連結成分を Biconnected Component とも呼んだりする

  # Complexity (n = |V|, m = |E|)
    Time: O(n + m)
    Memory: O(n + m)

  # Usage
    - Graph g(n): 頂点数 n の無向グラフを構成
    - g.add_edge(u, v): g に辺 {u, v} を追加
    - g.Bridge(): g を2辺連結成分分解して橋の数を返す
    - e = g.bridge[i]: i番目の橋 {e.first, e.second}
    - g.ord[v]: 頂点 v のDfsで訪れた順番
    - g.low[v]: 頂点 v の lowlink (lowlink が同じ値の頂点は同じ2辺連結成分に属する)

  # Description
    無向グラフ G に対して，深さ優先探索(Dfs)をして，各頂点 v の訪れた頂点番号を ord[v] とする．
    また，各頂点 v に対して，lowlink low[v] を v から Dfs して訪れることが可能な頂点の ord
    の最小値とする．ただし，葉から1度だけ後退辺(backedge)を使用することができる．
    後退辺とはDfs木に含まれない辺のことで，橋とはならない．
    後退辺ではない {u, v} に対して，{u, v} が橋 <==>  ord[u] < low[v] が成り立つ．
    lowlink が同じ値の頂点は同じ2辺連結成分に属する．

  # References
    - [保坂和宏，グラフ探索アルゴリズムとその応用](http://hos.ac/slides/20110504_graph.pdf)

  # Verified
    - [AOJ GRL_3_B Connected Components - Bridges]
      (http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=GRL_3_B)
    - [Atcoder Beginner Contest 075 C - Bridges]
      (https://beta.atcoder.jp/contests/abc075/tasks/abc075_c)
*/

#include <iostream>
#include <vector>
#include <algorithm>

// -------------8<------- start of library -------8<------------------------
struct Graph {
    const int n;
    std::vector<std::vector<int>> adj;
    std::vector<int> ord, low;
    std::vector<std::pair<int, int>> bridge;

    explicit Graph(int _n) : n(_n), adj(n), ord(n, -1), low(n, -1) {}

    void add_edge(int u, int v) { adj[u].push_back(v); adj[v].push_back(u); }

    int Bridge() {
        for (int v = 0, idx = 0; v < n; ++v)
            if (ord[v] == -1) Dfs(-1, v, idx);

        for (int v = 0; v < n; ++v)
            for (int u : adj[v])
                if (ord[v] < low[u])
                    bridge.push_back({v, u});
        return bridge.size();
    }

    void Dfs(const int prev, const int cur, int &idx) {
        low[cur] = ord[cur] = idx++;
        for (int v : adj[cur]) {
            if (ord[v] == -1) {
                Dfs(cur, v, idx);
                low[cur] = std::min(low[cur], low[v]);
            }
            else if (v != prev) low[cur] = std::min(low[cur], ord[v]);
        }
    }
};
// -------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    int n, m, v[2];
    std::cin >> n >> m;

    Graph g(n);
    for (int i = 0; i < m; ++i) {
        std::cin >> v[0] >> v[1];
        g.add_edge(v[0], v[1]);
    }

    g.Bridge();

    // Output for AOJ GRL_3_B: Bridge
    for (auto &e : g.bridge) if (e.second < e.first) std::swap(e.first, e.second);
    std::sort(g.bridge.begin(), g.bridge.end());
    for (auto e : g.bridge) std::cout << e.first << ' ' << e.second << '\n';

    return 0;
}
