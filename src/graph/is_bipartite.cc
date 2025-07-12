/*
  Recognition problem of bipartite graph
  二部グラフの認識問題
  ======================================

  # Problem
    Input: 無向グラフ G = (V, E)
    Output: G が二部グラフであるかどうか

    Def. 二部グラフ（Bipartite graph）
      無向グラフ G = (V, E) が二部グラフであるとは，V の分割 A, B で，任意の e \in E
      に対して，e の端点が A と B にちょうど1つずつ含まれるようなが存在すること

  # Complexity (n = |V|, m = |E|)
    - Time: O(n + m)
    - Memory: O(n + m)

  # Usage
    - Graph g(n): 頂点数 n の無向グラフを構築
    - g.add_edge(u, v): 辺 {u, v} を追加
    - IsBipartite(g): g が二部グラフかどうかを判定する

  # Description
    二部グラフの有名な特徴付けとして次が知られている．
      1. G が二部グラフ <=> G は 2彩色可能
      2. G が二部グラフ <=> G は長さ奇数の閉路を部分グラフとして持たない

    ここでは，1番目の特徴付けに基づいてアルゴリズムを構成している．
    すなわち，任意の頂点から深さ優先探索で貪欲彩色を行う．

  # Verified
    @tmaehara さんの実装とランダムグラフ上で比較
    (https://github.com/spaghetti-source/algorithm/blob/master/graph/is_bipartite.cc)
*/

#include <iostream>
#include <vector>
#include <functional>

// -------------8<------- start of library -------8<------------------------
struct Graph {
    const int n;
    std::vector<std::vector<int>> adj;
    explicit Graph(int _n) : n(_n), adj(n) {}
    void add_edge(int u, int v) { adj[u].push_back(v); adj[v].push_back(u); }
};

bool IsBipartite(const Graph &g) {
    enum class Color : char { Red, Blue, UNCOLOR };
    std::vector<Color> color(g.n, Color::UNCOLOR);

    std::function<bool(int, Color)> Dfs = [&](int v, Color c) {
        color[v] = c;
        Color diff_c = (c == Color::Red ? Color::Blue : Color::Red);
        for (const int u : g.adj[v]) {
            if (color[u] == c) return false;
            if (color[u] == Color::UNCOLOR && !Dfs(u, diff_c)) return false;
        }
        return true;
    };

    for (int v = 0; v < g.n; ++v)
        if (color[v] == Color::UNCOLOR && !Dfs(v, Color::Red)) return false;
    return true;
}
// -------------8<------- end of library ---------8-------------------------

int main() {
    int n, m, v[2];
    std::cin >> n >> m;

    Graph g(n);
    for (int i = 0; i < m; ++i) {
        std::cin >> v[0] >> v[1];
        g.add_edge(v[0], v[1]);
    }

    std::cout << (IsBipartite(g) ? "Yes " : "No ") << "Bipartite Graph\n";

    return 0;
}
