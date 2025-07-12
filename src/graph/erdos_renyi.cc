/*
  Erdos-Renyi Random Graph
  エルデシュ・レニィーランダムグラフ
  ===========================

  # Problem
    Input: 非負整数 n，実数 p (0 <= p <= 1)
    Output: 確率分布 G(n, p) に従って無向グラフ G = (V, E) を1つ選ぶ

    Def. Erdos-Renyi ランダムグラフ G(n, p)
      確率分布 G(n, p) の母集団は，集合 {1, ..., n} を頂点集合とする無向グラフ全体であり，
      G(n, p) の根元事象 G = (V, E) とすると，G が生起する確率を
      　　Pr[G] = p^|E| * (1 - p)^{choose(n, 2) - |E|}
      と定めたものである．
      確率分布 G(n, p) を Erdos-Renyiランダムグラフと呼ぶ．

  # Complexity (n = |V|, m = |E|)
    - Time: O(n^2)
    - Space: O(n + m)

  # Usage
    - Graph g = GenerateRandomGraph(n, p): Erdos-Renyiランダムグラフに従って無向グラフ g を生成

  # Description
    構成方法としては，任意の2頂点間に確率 p で辺を張るかどうかを一様乱数に従って行っていく．
    G(n, p) の性質としては次のようなものが知られている．
    　- 辺数 |E| の期待値は，p * choose(n, 2)
    　- 次数 k の頂点数の期待値は，choose(n - 1, k) * p^k * (1 - p)^{n - k - 1} * n

  # Note
  　確率分布 G(n, p) のことを Erdos-Renyiランダムグラフと呼ぶが，これを提案したのは Gilbert(1959)
  　である．Erdos と Renyi が提案したランダムグラフは，パラメータを頂点数 n と辺数 m として，
  　確率分布の母集団を集合 {1, ..., n} を頂点集合として，辺数が m となる無向グラフ全体としたものである．

  # References
    - 岡本吉央，情報数理工学実験第二 A・B 第2ラウンド「ネットワーク可視化のアルゴリズム」，2015年．
*/

#include <iostream>
#include <vector>
#include <random>

// -------------8<------- start of library -------8<------------------------
struct Graph {
    int n, m;
    std::vector<std::vector<int>> adj;
    explicit Graph(int _n) : n(_n), m(0), adj(n) {}
    void add_edge(int u, int v) { ++m; adj[u].push_back(v), adj[v].push_back(u); }
};

Graph GenerateRandomGraph(const int n, const double p) {
    std::random_device seed_gen;
    std::mt19937 engine(seed_gen());
    std::uniform_real_distribution<> dist(0.0, 1.0);

    Graph g(n);
    for (int v = 0; v < n; ++v)
        for (int u = v + 1; u < n; ++u)
            if (dist(engine) <= p) g.add_edge(u, v);
    return g;
}
// -------------8<------- end of library ---------8-------------------------

int main() {
    int n;
    double p;
    std::cin >> n >> p;

    Graph g = GenerateRandomGraph(n, p);
    std::cout << g.n << " " << g.m << '\n';
    for (int v = 0; v < g.n; ++v)
        for (int u : g.adj[v])
            if (v < u) std::cout << v << ' ' << u << '\n';

    return 0;
}
