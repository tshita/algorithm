/*
  2-vertex connected components
  2点連結成分(関節点の列挙)
  =============================

  # Problem
    Input: 無向グラフ G = (V, E)
    Output: G のすべての関節点と2点連結成分

    Def. 関節点(articulation point)
      G の関節点とは v \in V で，G - v の連結成分数が G よりも多くなるもの

    Def. 2点連結成分(2-vertex connected component)
      G の2点連結成分とは頂点部分集合で関節点が存在しない極大なもの

  # Complexity (n = |V|, m = |E|)
    Time: O(n + m)
    Memory: O(n + m)

  # Usage
    - Graph g(n): 頂点数 n の無向グラフを構成
    - g.add_edge(u, v): g に辺 {u, v} を追加
    - g.ArticulationPoints(): g の関節点の数を返す
    - g.ord[v]: 頂点 v のDfsで訪れた順番
    - g.low[v]: 頂点 v の lowlink
    - g.art[i]: i番目の関節点
    - g.BiconnectedComponent: g を2点連結成分分解する(実行前にArticulationPointsを行う)
    - g.comp.find(u, v): 頂点 u, v が同じ2点連結成分に属するか
    - g.is_same(u, v): g.comp.find(u, v) を呼び出す

  # Description
    無向グラフ G に対して，深さ優先探索(Dfs)をして，各頂点 v の訪れた頂点番号を ord[v] とする．
    また，各頂点 v に対して，lowlink low[v] を v から Dfs して訪れることが可能な頂点の ord
    の最小値とする．ただし，葉から1度だけ後退辺(backedge)を使用することができる．
    後退辺とはDfs木に含まれない辺のことである．
    関節点となるための必要十分条件は，
      Dfs木の根 r が関節点 <=> r の次数が1より大きい
      Dfs木の根以外の頂点 v が関節点 <=> Dfs木での v のある子 u で ord[v] <= low[u] を満たす

    2点連結成分は UnionFind で端点に関節点を含まない辺の端点を連結すると得られる．

  # References
    - [保坂和宏，グラフ探索アルゴリズムとその応用](http://hos.ac/slides/20110504_graph.pdf)

  # Verified
    - [AOJ GRL_3_A Connected Components - Articulation Points]
      (http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=GRL_3_A)
*/

#include <iostream>
#include <vector>
#include <algorithm>

//  @require ./data_structure/union_find.cc

// -------------8<------- start of library -------8<------------------------
struct Graph {
    const int n;
    std::vector<std::vector<int>> adj;
    std::vector<int> ord, low, art;

    explicit Graph(int _n) : n(_n), adj(n), ord(n, -1), low(n, -1) {}

    void add_edge(int u, int v) { adj[u].push_back(v); adj[v].push_back(u); }

    int ArticulationPoints() {
        for (int v = 0, idx = 0; v < n; ++v)
            if (ord[v] == -1) Dfs(-1, v, idx);
        return art.size();
    }

    void Dfs(const int prev, const int cur, int &idx) {
        low[cur] = ord[cur] = idx++;
        int deg = 0;
        for (int v : adj[cur]) {
            if (ord[v] == -1) {
                Dfs(cur, v, idx);
                low[cur] = std::min(low[cur], low[v]);
                if (prev == -1) ++deg;
                else if (deg == 0 && ord[cur] <= low[v]) {
                    ++deg;
                    art.push_back(cur);
                }
            }
            else if (v != prev) low[cur] = std::min(low[cur], ord[v]);
        }
        if (prev == -1 && 1 < deg) art.push_back(cur);
    }

    // 2点連結成分分解 (必要なければコメントアウト)
    UnionFind comp;
    bool is_same(const int u, const int v) { return comp.find(u, v); }
    void BiconnectedComponent() {
        comp.data.assign(n, -1);
        std::vector<bool> is_art(n, false);
        for (int idx : art) is_art[idx] = true;
        for (int v = 0; v < n; ++v) {
            if (is_art[v]) continue;
            for (auto u : adj[v]) if (!is_art[u]) comp.unite(u, v);
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

    g.ArticulationPoints();

    // Output for AOJ GRL_3_A: Articulation Points
    std::sort(g.art.begin(), g.art.end());
    for (int v : g.art) std::cout << v << '\n';

    return 0;
}
