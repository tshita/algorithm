/*
  Vertex Cover Problem (using Maximal Mathcing) : 2-approximation algorithm
  頂点被覆問題（極大マッチングを使用）：2-近似アルゴリズム
  ==================================================================================

  # Problem
    Input: 無向グラフ G = (V, E)
    Output: G の頂点被覆

    Def. 頂点被覆（vertex cover）
      頂点部分集合 C \subseteq V で，どの辺 {u, v} \in E に対して，
      u \in C または v \in C が成り立つもののこと

  # Complexity (n = |V|, m = |E|)
    Time: O(n + m)
    Space: O(n + m)
    Approximation ratio: 2

  # Usage
    - vc MinimalVertexCover(g): g の頂点被覆 vc を返す（vc はサイズ n の std::vector<bool>）
    　　　　　　　　　　　　　　　　　　　vc[v] := v が頂点被覆に含まれるかどうか
    - bool CheckVertexCover(g, vc): vc が g の頂点被覆かどうかを判定

  # Description
    極大マッチングを使用した2-近似多項式時間アルゴリズム．
    G の極大マッチング M を貪欲的に求めて M の端点の集合を頂点被覆 C として出力する．

    Prop. G の最小頂点被覆 C* に対して，|C| <= 2 |C*|
    証明：
    　任意の辺 {u, v} \in M に対して，C* は頂点被覆なので u \in C* または v \in C* が成り立つ．
    　また，M はマッチングなので異なる M の2辺を被覆する C* の頂点は異なる．
    　したがって，|M| <= |C*| が成り立つ．このとき，2|M| = |C| であるから，|C| <= 2|C*| となる．

  # References
    - [Wikipedia: Vertex cover Approximate evaluation]
      (https://en.wikipedia.org/wiki/Vertex_cover#Approximate_evaluation)
*/

#include <iostream>
#include <vector>
#include <algorithm>

// -------------8<------- start of library -------8<------------------------
struct Graph {
    int n;
    std::vector<std::vector<int>> adj;
    explicit Graph(int _n) : n(_n), adj(n) {}
    void add_edge(const int u, const int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
};

std::vector<bool> MinimalVertexCover(const Graph &g) {
    std::vector<bool> vc(g.n, false);
    for (int v = 0; v < g.n; ++v) {
        for (int u : g.adj[v])
            if (!vc[v] && !vc[u]) vc[v] = vc[u] = true;
    }
    return vc;
}

bool CheckVertexCover(const Graph &g, const std::vector<bool> &vc) {
    for (int v = 0; v < g.n; ++v)
        for (int u : g.adj[v])
            if (!vc[v] && !vc[u]) return false;
    return true;
}
// -------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    int n, m;
    std::cin >> n >> m;

    Graph g(n);
    for (int i = 0, u, v; i < m; ++i) {
        std::cin >> u >> v;
        g.add_edge(u, v);
    }

    auto vc = MinimalVertexCover(g);
    std::cout << "Is vc is a vertex cover?: "
              << (CheckVertexCover(g, vc) ? "Yes" : "No") << std::endl;
    std::cout << "The size of the vertex cover is "
              << std::count(vc.begin(), vc.end(), true) << std::endl;

    return 0;
}
