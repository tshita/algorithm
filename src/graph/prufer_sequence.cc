/*
  Prüfer sequence (Heinz Prüfer, 1918)
  プリューファ列（ハインツ・フリューファ）
  ====================================

  # Problem
    Input: 頂点数 n のラベル付き木 T
    Output: T のプリューファ列 p = (p_1, p_2, ..., p_{n-2})

    Input: プリューファ列 p = (p_1, p_2, ..., p_{n-2})
    Output: p に対応する頂点数 n のラベル付き木 T

  # Complexity
    - Time: O(n log n)
    - Space: O(n)

  # Usage
    - std::vector<int> TreeToPruferSequence(Grah tree)
  　　　: ラベル付き木 tree に対応する prüfer sequence を返す
  　- Graph PruferSequenceToTree(std::vector<int> seq)
  　　　: prüfer sequence seq に対応するラベル付き木を返す

  # Description
    頂点数 n のラベル付き木 T に対応する prüfer sequence とは長さ n - 2 の整数列 p で次のように構成される．
    ここで，T のラベルはすべて異なり，1 から n までの値が割り当てられているとする．
    T のラベル最小の葉 v を選び，v の隣接点 u を p に追加する．そして，v を削除するということを，
    p のサイズが n - 2 となるまで繰り返す．

    prüfer sequence p = (p_1, ..., p_{n-2}) からラベル付き木 T への逆変換は次のようにする．
    まず，各頂点 v の p での出現回数 + 1 が T での v の次数と等しくなる．
    よって，各頂点の次数が分かるので上の変換と同様に，次数が1でラベル最小の頂点 v と p_1 を辺で結び，
    v と p_1 の次数を1減らすということを p_{n-2} まで行う．
    この操作を繰返した後に，次数が 0 ではない頂点がちょうど2つ存在するのでその間に辺を加えて T を構成する．
    この逆変換はもと変換の逆写像となるので一対一対応となる．

  # Note
    - ケイリーの公式：頂点数 n のラベル付き木の数は n^{n-2} という証明が prüfer sequence で示せる
    - 一様ランダムな頂点数 n のラベル付き木は prüfer sequence をランダムに生成して変換すると生成できる

  # References
    - [Wikipedia: Prüfer sequence](https://en.wikipedia.org/wiki/Pr%C3%BCfer_sequence)

  # Verified

*/

#include <iostream>
#include <vector>
#include <queue>

// -------------8<------- start of library -------8<------------------------
struct Graph {
    size_t n;
    std::vector<std::vector<int>> adj;

    explicit Graph(size_t _n) : n(_n), adj(_n) {}
    void add_edge(const int u, const int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
};

std::vector<int> TreeToPruferSequence(const Graph &tree) {
    if (tree.n <= 2) return std::vector<int>{};

    std::priority_queue<int, std::vector<int>, std::greater<int>> que;
    std::vector<int> deg(tree.n);
    for (size_t v = 0; v < tree.n; ++v) {
        deg[v] = tree.adj[v].size();
        if (deg[v] == 1) que.push(v);
    }

    std::vector<int> seq; seq.reserve(tree.n - 2);
    while (seq.size() + 2 < tree.n) {
        const int leaf = que.top(); que.pop();
        for (int u : tree.adj[leaf]) {
            if (deg[u] != 0) {
                seq.push_back(u);
                --deg[u];
                if (deg[u] == 1) que.push(u);
                break;
            }
        }
        --deg[leaf];
    }

    return seq;
}

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
// -------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    int size;
    std::cin >> size;

    std::vector<int> prufer_sequence(size);
    for (auto &&v : prufer_sequence) std::cin >> v;

    auto tree = PruferSequenceToTree(prufer_sequence);
    std::cout << "#vertices: " << tree.n << '\n';
    for (size_t v = 0; v < tree.n; ++v) {
        std::cout << v << ": ";
        for (auto u : tree.adj[v]) std::cout << u << " ";
        std::cout << '\n';
    }

    return 0;
}
