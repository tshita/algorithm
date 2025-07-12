/*
  Generating Random Labelled Tree: Prüfer Sequence
  ラベル付き木を一様ランダムに生成： Prüfer sequence
  =====================================================

  # Problem
    Input: 非負整数 n
    Output: n 頂点のラベル付き木を一様ランダムに生成

  # Complexity
    - Time: O(n log n)
    - Space: O(n)

  # Usage
    - Graph RandomLabelledTree(n): n 頂点のラベル付き木を一様ランダムに生成して返す
    - Edges ConvertEdges(g): g を辺の集合に変換（分布を調べるために）

  # Description
    n 頂点のラベル付き木の数はケイリーの公式より n^{n-2} 個あるので，一様ランダムに生成するとは
    各ラベル付き木が選ばれる確率を 1 / n^{n-2} に定めることである．
    prüfer sequence をランダムに生成してラベル付き木に変換して生成している．

  # Note
    - ラベルなし木を一様ランダムに生成しているわけでは無いので注意（木が選ばれる確率が異なる）

  # References
    - graph/prufer_sequence.cc を参照

  # Verified

*/

#include <cstdio>
#include <map>
#include <vector>
#include <queue>
#include <random>
#include <algorithm>

// -------------8<------- start of library -------8<------------------------
struct Graph {
    int n;
    std::vector<std::vector<int>> adj;
    explicit Graph(size_t _n) : n(_n), adj(_n) {}
    void add_edge(int u, int v) { adj[u].push_back(v); adj[v].push_back(u); }
};

Graph PruferSequenceToTree(const std::vector<int> &seq) {
    const int n = seq.size() + 2;
    std::vector<int> deg(n, 1);
    for (const int v : seq) ++deg[v];

    std::priority_queue<int, std::vector<int>, std::greater<int>> que;
    for (int v = 0; v < n; ++v) if (deg[v] == 1) que.push(v);

    Graph tree(n);
    for (const int v : seq) {
        const int u = que.top(); que.pop();
        tree.add_edge(v, u);
        --deg[v]; --deg[u];
        if (deg[v] == 1) que.push(v);
    }

    const int lst = que.top(); que.pop();
    tree.add_edge(lst, que.top());

    return tree;
}

Graph RandomLabelledTree(const int n) {
    if (n <= 1) return Graph(1);

    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());
    std::uniform_int_distribution<> dist(0, n - 1);

    std::vector<int> seq(n - 2);
    for (int i = 0; i < n - 2; ++i) seq[i] = dist(engine);

    return PruferSequenceToTree(seq);
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

int main() {
    int n, sample_size;
    scanf("%d %d", &n, &sample_size);

    // 非同型なラベル付き木の数を数える
    std::map<Edges, int> cnt;
    for (int i = 0; i < sample_size; ++i)
        ++cnt[ConvertEdges(RandomLabelledTree(n))];

    printf("%zu (cayley's formula: %d)\n", cnt.size(), (int)std::pow(n, n - 2));
    for (const auto &it : cnt) // 各ラベル付き木の生成確率を出力
        printf("%.4f ", (double)it.second / sample_size);
    puts("");

    return 0;
}
