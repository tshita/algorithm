/*
  Generating Uniform Spanning Tree: Wilson's Algorithm
  一様全域木の生成： ウィルソンのアルゴリズム
  ====================================================

  # Problem
    Input: 無向グラフ G = (V, E)
    Output: G の spanning tree を等確率で生成

  # Complexity
    - Time: たぶん O(n^3)
    - Space: O(n)

  # Usage
    - Graph RandomLabelledTree(n): 頂点数 n のラベル付き木を一様ランダムに生成して返す

    - Edges ConvertEdges(g): g を辺の集合に変換（分布を調べるために）
    - bool CheckSpanningTree(tree): tree が木かどうかを判定

  # Description
    G の全域木で等確率に選ばれたものを G の uniform spanning tree（一様全域木）と呼ぶ．

    Wilson's algorithm は loop-erased random walk を基に与えられた無向グラフの全域木を
    一様ランダムに生成する．無向グラフが頂点数 n の完全グラフのとき，生成されるものは頂点数 n の
    ラベル付き木となる．頂点数 n のラベル付き木の総数は Cayley's formula より n^{n-2} なので，
    生成される各ラベル付き木は確率 1 / n^{n-2} で選ばれる．

    無向グラフ G の歩道を w = (w_1, ..., w_k) とする．w には多重辺や自己ループが含まれているかもしれない．
    w の loop erasure は G の単純道 p = (w_{i_1}, w_{i_2}, ..., w_{i_l}) は，
    　　・i_1 = 1
    　　・i_{j+1} = max{i : w_i = w_{i_{j}}}
    となる．
    G の random walk w の loop erasure を w の loop-erased random walk と呼ぶ．
    random walk は任意の頂点からスタートして，現在頂点 v にいるとき各隣接頂点を確率 1 / d(v)
    で選びながら進む歩道である（d(v) は v の次数）．
    V = {v_1, v_2, ..., v_n} としたとき，D_1 = {v_1} として，次を繰り返す．
      for (i = 2; i < n; ++i) {
        ・ v_i から D_{i - 1} のどれかの頂点への random walk をする．
        ・ この random walk を w_i として，w_i の loop-erased random walk を p_i とする．
        ・ D_i = D_{i - 1} \cup p_i とする
      }
      p_i の集合が G の全域木でこれを出力する

  # Note
    - 計算時間がよくわからないが期待値計算時間はたぶん多項式
    - G の全域木の数の近似値を求めることができる（サンプル数が多くなるので行列木定理を用いる方がよい）

  # References
    - [Kohei Ozaki, Random Walk Proof of Kirchhoff's Matrix Tree Theorem]
      (https://i.ho.lc/random-walk-proof-of-kirchhoffs-matrix-tree-theorem.html)
    - [Wikipedia: Loop-erased random walk]
      (https://en.wikipedia.org/wiki/Loop-erased_random_walk)

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

    std::vector<bool> selected(g.n, false);
    selected[0] = true;
    for (int v = 1; v < g.n; ++v) {
        if (selected[v]) continue;

        std::vector<int> max_time(g.n);
        int cur = v;
        for (int time = 1; !selected[cur]; ++time) {
            max_time[cur] = time;
            int nxt = g.adj[cur][dist(g.adj[cur].size())];
            if (selected[nxt]) max_time[nxt] = ++time;
            cur = nxt;
        }

        for (cur = v; !selected[cur]; ) {
            int nxt = -1;
            for (const int u: g.adj[cur]) {
                if (max_time[cur] < max_time[u]) {
                    nxt = u;
                    if (max_time[cur] + 1 == max_time[u]) break;
                }
            }

            selected[cur] = true;
            tree.add_edge(cur, nxt);
            cur = nxt;
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
