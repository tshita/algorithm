/*
  Generating Uniform Spanning Tree: Aldous's Algorithm
  一様全域木の生成： アルドゥスのアルゴリズム
  ====================================================

  # Problem
    Input: 無向グラフ G = (V, E)
    Output: G の spanning tree を等確率で生成

  # Complexity (n = |V|, m = |E|, C = cover time)
    - Time: O(m C)
      - General graphs: m C <= n^3
      - Regular Graphs: m C <= 8n^2
    - Space: O(n)

  # Usage
    - Graph RandomLabelledTree(n): 頂点数 n のラベル付き木を一様ランダムに生成して返す

    - Edges ConvertEdges(g): g を辺の集合に変換（分布を調べるために）
    - bool CheckSpanningTree(tree): tree が木かどうかを判定

  # Description
    G の全域木で等確率に選ばれたものを G の uniform spanning tree（一様全域木）と呼ぶ．

    任意の頂点からすべての頂点を訪れるまで random walk を行う．
    頂点 u から v へ移動して，v が初めて訪れたときに，生成する全域木の辺として {u, v} を加える．
    このときにかかったステップ数を conver time と呼ぶ．

    random walk とは，現在いる頂点 v から各隣接頂点を確率 1 / d(v) （d(v) は v の次数）
    で選び移動するということを繰返したときの訪れた歩道のことをいう．

  # Note
    - 実行時間は期待値であることに注意

  # References
    - David J Aldous,"The Random Walk Construction of Uniform Spanning Trees
      and Uniform Labelled Trees", SIAM J. Discrete Math, 1990.

  # Verified

*/

#include <cstdio>
#include <vector>
#include <algorithm>
#include <random>
#include <map>
#include <stack>

// -------------8<------- start of library -------8<------------------------
struct Graph {
    int n;
    std::vector<std::vector<int>> adj;
    explicit Graph(int _n) : n(_n), adj(_n) {}
    void add_edge(int u, int v) { adj[u].push_back(v); adj[v].push_back(u); }
};

Graph UniformSpanningTree(const Graph &g) {
    std::random_device seed_gen;
    std::mt19937 engine(seed_gen());
    auto dist = [&engine](const int deg) {
        return std::uniform_int_distribution<>(0, deg - 1)(engine);
    };

    Graph tree(g.n);
    std::vector<bool> visited(g.n, false);
    visited[0] = true;
    for (int cur = 0, num = 1, nxt; num < g.n; cur = nxt) {
        nxt = g.adj[cur][dist(g.adj[cur].size())];
        if (!visited[nxt]) {
            visited[nxt] = true;
            tree.add_edge(cur, nxt);
            ++num;
        }
    }

    return tree;
}
// -------------8<------- end of library ---------8-------------------------

// 非同型なラベル付き木の数を数えるため辺の集合に変換
using Edges = std::vector<std::pair<int, int>>;
Edges ConvertEdges(const Graph &g) {
    Edges edges;
    for (int v = 0; v < g.n; ++v)
        for (int u : g.adj[v]) if (v < u) edges.emplace_back(std::make_pair(v, u));
    std::sort(edges.begin(), edges.end());
    return edges;
}

// tree が木かどうかを判定（G の部分グラフは成り立つ）
bool CheckSpanningTree(const Graph &tree) {
    int m = 0;
    for (int v = 0; v < tree.n; ++v) m += tree.adj[v].size();
    if (m != 2 * (tree.n - 1)) return false;

    std::vector<bool> visited(tree.n, false);
    std::stack<int> st;
    st.push(0); visited[0] = true;
    while (!st.empty()) {
        const int cur = st.top(); st.pop();
        for (auto u : tree.adj[cur])
            if (!visited[u]) {
                visited[u] = true;
                st.push(u);
            }
    }
    return std::count(visited.begin(), visited.end(), true) == tree.n;
}

int main() {
    int n, m, sample_size;
    scanf("%d %d %d", &n, &m, &sample_size);

    Graph g(n);
    for (int i = 0, u, v; i < m; ++i) {
        scanf("%d %d", &u, &v);
        g.add_edge(u, v);
    }

    // G の uniform spanning tree を数える
    std::map<Edges, int> cnt;
    for (int i = 0; i < sample_size; ++i) {
        const auto tree = UniformSpanningTree(g);
        if (!CheckSpanningTree(tree)) puts("Error");
        ++cnt[ConvertEdges(tree)];
    }

    printf("%zu\n", cnt.size());
    for (const auto &it : cnt) // 各ラベル付き木の生成確率を出力
        printf("%.4f ", (double)it.second / sample_size);
    puts("");

    return 0;
}
