/*
  Eulerian Graph (Hierholzer's Algorithm)
  オイラーグラフのハイヤーホルザのアルゴリズム
  ========================================

  # Problem
    Input: 多重無向グラフ G = (V, A)
    Output: G がオイラーグラフか準オイラーグラフかを判定して，存在するならばオイラー路かオイラー閉路を求める

    Def. オイラー路 (Eulerian path)
      G のオイラー路とは，G のすべての辺をちょうど1回通る道

    Def. 準オイラーグラフ (semi-Eulerian graph)
      G にオイラー路が存在するとき，G をオイラーグラフと呼ぶ

    Def. オイラー閉路 (Eulerian cycle)
      G のオイラー閉路とは，G のすべての辺をちょうど1回通る閉路

    Def. オイラーグラフ (Eulerian graph)
      G にオイラー閉路が存在するとき，G をオイラーグラフと呼ぶ

  # Complexity (n = |V|, m = |A|)
    - Time: O(n + m)
    - Space: O(n + m)

  # Usage
    - Graph g(n): 頂点数 n の無向グラフを構築
    - g.add_edge(u, v): g に辺 {u, v} を追加
    - g.IsEulerianGraph(): g がオイラーグラフか準オイラーグラフかどうかを判定する(隣接リストを破壊)
    - g.path: g の オイラー路かオイラー閉路の訪れた頂点の列

  # Description
    下の連結性についての条件は，G の孤立点を取り除いたものを考える．

    Thm. オイラーグラフ
      グラフ G がオイラーグラフであるための必要十分条件は，G が連結で，かつ，すべての頂点の次数が偶数であること

    Thm. 準オイラーグラフ
      グラフ G が準オイラーグラフであるための必要十分条件は，G が連結で，かつ，次数が奇数の頂点が 2 つと，
      それ以外の頂点の次数が偶数であること

    証明は有向グラフの場合と同様に示すことができる．
    有向グラフの実装と異なり，辺を訪問済みにする処理のために逆辺のインデックスを持つ．

  # Note
    グラフの隣接リストを破壊するので注意が必要

  # References
    + [Learning Algorithms](http://www.learning-algorithms.com/entry/2017/12/07/193238)
    + [GeeksforGeeks](https://www.geeksforgeeks.org/hierholzers-algorithm-directed-graph/)

  # Verified
    + Eulerian path
      - [AOJ 0086](http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=0086)
*/

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>

// -------------8<------- start of library -------8<------------------------
struct Graph {
    struct Edge {
        int dst, ridx;
        Edge() {}
        Edge(int _d, int _r) : dst(_d), ridx(_r) {}
    };

    const int n;
    size_t m = 0, start = 0;
    std::vector<std::vector<Edge>> adj;
    std::vector<int> deg;

    explicit Graph(int _n) : n(_n), adj(_n), deg(_n) {}

    void add_edge(int src, int dst) {
        adj[src].emplace_back(Edge(dst, deg[dst]++));
        adj[dst].emplace_back(Edge(src, deg[src]++));
        ++m; start = src;
    }

    std::vector<int> path;
    bool IsEulerianGraph() {
        int num_odd = 0;
        for (int v = 0; v < n; ++v) if (deg[v] % 2 == 1) { ++num_odd; start = v; }
        if (2 < num_odd) return false;

        path.reserve(m + 1);
        std::function<void(int)> dfs = [&](int cur) {
            while (!adj[cur].empty()) {
                const auto &e = adj[cur].back();
                adj[adj[e.dst].back().dst][adj[e.dst].back().ridx].ridx = e.ridx;
                std::swap(adj[e.dst][e.ridx], adj[e.dst].back());
                adj[cur].pop_back();
                adj[e.dst].pop_back();
                dfs(e.dst);
            }
            path.push_back(cur);
        };

        dfs(start);
        std::reverse(path.begin(), path.end());
        return (path.size() == m + 1);
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

    if (g.IsEulerianGraph()) {
        std::cout << "Eulerian graph\n";
        for (auto v : g.path) std::cout << v << " ";
        std::cout << std::endl;
    }
    else std::cout << "Not Eulerian graph.\n";

    return 0;
}
