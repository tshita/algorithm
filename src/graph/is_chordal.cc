/*
  Chordal Graph Recognition Problem
  弦グラフの認識問題
  =================================

  # Problem
    Input: Undirected Graph G = (V, E)
    Output: Is G chordal graph?

  # Complexity
    Time and Space: O(|V| + |E|)

  # Usage
    bool IsChordal(vector<vector<int>> G)

  # Description
    LexBfsで頂点の線形順序oを得る．o が perfect elimination ordering(PEO)
    ならば，G は chordal graph である．o が PEO であるとは，任意の v \in V に対して，
    {u \in V : o(u) < o(v) and {u, v} \in E} がクリークである．

  # Note
  # References

  # Verified
    いくつかのグラフに対して Python の NetworkX の is_chordal(G) と比較をした
*/

#include <iostream>
#include <vector>
#include <algorithm>

using Graph = std::vector<std::vector<int>>;

//  @require ./lexicographic_bfs.cc 👇👇

// --------------8<------- start of library -------8<--------------------
bool IsChordal(const Graph &g) {
    const int n = g.size();
    std::vector<int> idx(n), adj(n, -1), order = LexBfs(g);

    for (int i = 0; i < n; ++i) idx[order[i]] = i;

    for (const int v : order) {
        int p = -1, size = 0;

        for (const int u : g[v]) {
            if (idx[u] < idx[v]) {
                adj[u] = v;
                ++size;
                p = std::max(p, idx[u]);
            }
        }
        for (const int u : g[order[p]])
            if (adj[u] == v) --size;

        if (p != -1 && size != 1) return false;
    }

    return true;
}
// ----------------8<------- end of library -------8<--------------------


int main() {
    // Input Undirected Graph G = (V, E)
    // n := |V|, m := |E|
    int n, m, v[2];
    std::cin >> n >> m;

    Graph g(n);
    for (int i = 0; i < m; ++i) {
        std::cin >> v[0] >> v[1];
        for (int j = 0; j <= 1; ++j)
            g[v[j]].push_back(v[1 - j]);
    }

    // Output LBFS ordering of vertices V
    std::cout << (IsChordal(g) ? "Yes " : "No ") << "Chordal Graph\n";

    return 0;
}
