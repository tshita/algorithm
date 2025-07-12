/*
  Lexicographic Breadth First Search (Rose, Tarjan, Lueker 1976)
  辞書順幅優先探索
  ==============================================================

  # Problem
    Input: Undirected Graph G = (V, E)
    Output: V上の線形順序 (LexBfs)

  # Complexity
    Time and Space: O(|V| + |E|)

  # Usage
    - vector<int> LexBfs(vector<vector<int>> G)
    - 戻り値 vector<int> ord : ord[i] = v は i番目(0-index)に訪れた頂点番号(0-index)

  # Description
    G の Lexicographic Bradth First Search(LexBfs) とは BFS の一種である．
    任意の頂点を始点としてBFSを行うが，次に訪れる頂点は，未訪問の頂点で，その頂点に隣接している
    訪問済みの頂点らの訪れた順番の辞書式順序が最も小さいものが選ばれる．
    例えば，未訪問の頂点が2つあり，それぞれの隣接している訪問済み頂点の訪れた順番が 1,3,4 と
    1,3,6 のときは前者が次に選ばれる．
    線形時間で実行するために Partition Refinement を実装した．

  # Note
    - スマートポインタ(std::shared_ptr)を使うと遅くなるけど使用
    - 何故か遅いので調査(|V| = 10000, |E| = 30000000 ぐらいで 6.5 [s])
      -> 多分定数倍が重いはず（∵ antaさんのライブラリと比較しても同程度の速度）

  # References
    Derek G. Corneil : Lexicographic Breadth First Search -- A Survey.
    International Workshop on Graph-Theoretic Concepts in Computer Science,
    Bad Honnef, Germany, June 21-23, 2004, pp. 1--19.

    [@anta ChordalGraph.cpp](https://www.dropbox.com/sh/9lknvq4xay709cn/AAC4CXtIHvzPoIJjJuZbYfZKa?preview=%23ChordalGraph.cpp)

  # Verified

*/

#include <iostream>
#include <vector>
#include <memory>

// ------------8<------- start of library -------8<-------------------------------
struct Graph {
    const int n;
    std::vector<std::vector<int>> adj;
    explicit Graph(int _n) : n(_n), adj(_n) {}
    void add_edge(const int src, const int dst) { adj[src].push_back(dst); }
};

std::vector<int> LexBfs(const Graph &g) {
    struct Data {
        explicit Data(int _s) : size(_s), item(_s) {}
        std::shared_ptr<Data> prev = nullptr, nxt = nullptr;
        int size, size_new = 0;
        std::vector<int> item;
    };

    std::vector<int> order(g.n, -1);
    std::vector<char> visited(g.n, false); // vector<bool> is slow
    std::vector<std::pair<std::shared_ptr<Data>, int>> ptr(g.n);
    std::shared_ptr<Data> data = std::make_shared<Data>(g.n);
    for (int i = 0;  i < g.n; ++i) {
        ptr[i] = std::make_pair(data, i);
        data->item[i] = i;
    }

    std::shared_ptr<Data> head(data);
    int size_order = 0;
    while (size_order < g.n) {
        // choosing a pivot
        const int pivot = order[size_order++] = head->item[0];
        visited[pivot] = true;
        std::swap(head->item[0], head->item[head->size - 1]);
        std::swap(ptr[head->item[0]].second, ptr[head->item[head->size - 1]].second);
        --(head->size);
        if (head->size == 0) head = head->nxt;

        // selecting vertices to partition
        for (const auto u : g.adj[pivot]) {
            if (visited[u]) continue;

            std::shared_ptr<Data> cur(ptr[u].first);
            if (cur->size == 1) continue;

            const int idx = ptr[u].second;
            std::swap(cur->item[idx], cur->item[cur->size - cur->size_new - 1]);
            std::swap(ptr[u].second, ptr[cur->item[idx]].second);
            ++(cur->size_new);
        }

        // partitioning
        for (const auto u : g.adj[pivot]) {
            if (visited[u]) continue;

            std::shared_ptr<Data> cur(ptr[u].first);
            if (cur->size_new == 0 || cur->size == cur->size_new) {
                cur->size_new = 0;
                continue;
            }

            const int size_new = cur->size_new;
            std::shared_ptr<Data> new_data = std::make_shared<Data>(size_new);
            for (int i = 0; i < size_new; ++i) {
                new_data->item[i] = cur->item[cur->size - size_new + i];
                ptr[new_data->item[i]].first = new_data;
                ptr[new_data->item[i]].second = i;
            }

            if (head == cur) head = new_data;

            cur->size -= cur->size_new;
            cur->size_new = 0;
            new_data->prev = cur->prev;
            new_data->nxt = cur;
            if (new_data->prev != nullptr) new_data->prev->nxt = new_data;
            cur->prev = new_data;
        }
    }

    return order;
}
// -------------8<------- end of library -------8<-----------------------------


int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    // Input Undirected Graph G = (V, E)
    // n := |V|, m := |E|
    int n, m, v[2];
    std::cin >> n >> m;

    Graph g(n);
    for (int i = 0; i < m; ++i) {
        std::cin >> v[0] >> v[1];
        g.add_edge(v[0], v[1]);
        g.add_edge(v[1], v[0]);
    }

    // Output LBFS ordering of vertices V
    auto ord = LexBfs(g);
    for (int i = 0; i < n; ++i)
        std::cout << ord[i] << " \n"[i == n - 1];

    return 0;
}
