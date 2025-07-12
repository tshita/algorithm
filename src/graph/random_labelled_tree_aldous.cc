/*
  Generating Random Labelled Tree: Aldous's Algorithm
  ラベル付き木を一様ランダム生成： アルドゥスのアルゴリズム
  ====================================================

  # Problem
    Input: 非負整数 n
    Output: 頂点数 n のラベル付き木を一様ランダムに生成（各ラベル付き木が選ばれる確率が 1 / n^{n-2}）

  # Complexity
    - Time: O(n)
    - Space: O(n)

  # Usage
    - Graph RandomLabelledTree(n): 頂点数 n のラベル付き木を一様ランダムに生成して返す

    - Edges ConvertEdges(g): g を辺の集合に変換（分布を調べるために）
    - bool CheckSpanningTree(tree): tree が木かどうかを判定

  # Description
    Aldous's algorithm はランダムウォークをして与えられた無向グラフの全域木を一様ランダムに生成する．
    無向グラフが頂点数 n の完全グラフのとき，生成されるのは頂点数 n のラベル付き木となる．
    ラベル付き木を一様ランダムに生成する場合は Aldous's algorithm をシンプルにでき線形時間となる．

  # Note
    - ラベルなし木を一様ランダムに生成しているわけではないので注意（スターと道の確率が異なる）

  # References
    - David J Aldous,"The Random Walk Construction of Uniform Spanning Trees
      and Uniform Labelled Trees", SIAM J. Discrete Math, 1990.
    - [@anta_prg's source code in C++](https://ideone.com/1mQWNd)

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

Graph RandomLabelledTree(const int n) {
    std::random_device seed_gen;
    std::mt19937 engine(seed_gen());
    std::uniform_int_distribution<> dist(0, n - 1);

    Graph tree(n);
    std::vector<int> perm(n);
    std::iota(perm.begin(), perm.end(), 0);
    std::shuffle(perm.begin(), perm.end(), engine);

    for (int i = 1; i < n; ++i)
        tree.add_edge(perm[i], perm[std::min(i - 1, dist(engine))]);

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
    int n, sample_size;
    scanf("%d %d", &n, &sample_size);

    // 非同型なラベル付き木の数を数える
    std::map<Edges, int> cnt;
    for (int i = 0; i < sample_size; ++i) {
        const auto tree = RandomLabelledTree(n);
        if (!CheckSpanningTree(tree)) puts("Error");
        ++cnt[ConvertEdges(tree)];
    }

    long long caley = 1;
    for (int i = 0; i < n - 2; ++i) caley *= n;

    printf("%zu (cayley's formula: %lld)\n", cnt.size(), caley);
    for (const auto &it : cnt) // 各ラベル付き木の生成確率を出力
        printf("%.4f ", (double)it.second / sample_size);
    puts("");

    return 0;
}
