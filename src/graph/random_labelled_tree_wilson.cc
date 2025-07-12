/*
  Generating Random Labelled Tree: Wilson's Algorithm
  ラベル付き木を一様ランダム生成：ウィルソンのアルゴリズム
  ====================================================

  # Problem
    Input: 非負整数 n
    Output: 頂点数 n のラベル付き木を一様ランダムに生成（各ラベル付き木が選ばれる確率が 1 / n^{n-2}）

  # Complexity
    - Time: たぶん O(n^3)
    - Space: O(n)

  # Usage
    - Graph RandomLabelledTree(n): 頂点数 n のラベル付き木を一様ランダムに生成して返す
    - Edges ConvertEdges(g): g を辺の集合に変換（分布を調べるために）

  # Description
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
    - ラベルなし木を一様ランダムに生成しているわけではないので注意（スターと道の確率が異なる）
    - 計算時間がよくわからないが期待値計算時間はたぶん多項式

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

    std::vector<bool> selected(n, false);
    selected[0] = true;
    for (int v = 1; v < n; ++v) {
        if (selected[v]) continue;

        std::vector<int> max_time(n);
        int cur = v;
        for (int time = 1; !selected[cur]; ++time) {
            max_time[cur] = time;
            int nxt = dist(engine);
            while (nxt == cur) nxt = dist(engine);
            if (selected[nxt]) max_time[nxt] = ++time;
            cur = nxt;
        }

        for (cur = v; !selected[cur]; ) {
            int nxt = -1;
            for (int u = 0; u < n; ++u) {
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
