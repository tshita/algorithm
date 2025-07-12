/*
  Maximum Bipartite Matching (using Ford-Fulkerson)
  二部グラフの最大マッチング（フォード・ファルカーソン法を使用）
  =================================================

  # Problem
    Input: 二部グラフ G = (A; B, E)
    Output: G の最大マッチングのサイズ

    Def. マッチング(Matching)
      G のマッチングとは，辺部分集合 M \subseteq E で M のどの異なる2辺も端点が異なるもの．
      サイズが最大となるマッチングを最大マッチングと呼ぶ

  # Complexity
    Time : O(|V| * (|V| + |E|))
    Space : O(|V| + |E|)

  # Description
    For-Fulkersonのように増加道をDfsで見つけてくる

  # References
    [@tmaehara spagetthi-source/algorithm/graph/bipartite_matching]
    (https://github.com/spaghetti-source/algorithm/blob/master/graph/bipartite_matching.cc)

  # Verified
    [AOJ GRL_7_A Matching - Bipartite Matching]
    (http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=GRL_7_A)
*/

#include <iostream>
#include <vector>

// -------------8<------- start of library -------8<------------------------
class BipariteGraph {
public:
    // Bipartite Graph G = (A; B, E), A = [0, L), B = [L, L + R)
    BipariteGraph(int _L, int _R)
        : L(_L), R(_R), adj(L + R), match(L + R, -1), visited(L + R), size_match(0) {}

    void add_edge(const int v1, const int v2) {
        adj[v1].push_back(L + v2);
        adj[L + v2].push_back(v1);
    }

    int MaximumMatching() {
        for (int v = 0; v < L; ++v) {
            visited.assign(L + R, false);
            if (Augment(v)) ++size_match;
        }

        /* もし解が必要ならばコメントアウト */
        // vector<pair<int, int>> matching(size_match);
        // for (int v = 0, idx = 0; v < L; ++v)
        //     if (0 <= match[v])
        //         matching[idx++] = {v, match[v] - L};

        return size_match;
    }

private:
    const int L, R;
    std::vector<std::vector<int>> adj;
    std::vector<int> match;
    std::vector<char> visited; // vector<bool> is slow
    int size_match;

    bool Augment(const int cur) {
        for (const int dst : adj[cur])
            if (!visited[dst]) {
                visited[dst] = true;
                if (match[dst] < 0 || Augment(match[dst])) {
                    match[cur] = dst; match[dst] = cur;
                    return true;
                }
            }
        return false;
    }
};
// -------------8<------- end of library ---------8-------------------------

int main() {
    int L, R, m;
    std::cin >> L >> R >> m;

    // prtite sets : A = [0, L), B = [0, R)
    BipariteGraph g(L, R);
    for (int i = 0, v[2]; i < m; ++i) {
        std::cin >> v[0] >> v[1];
        g.add_edge(v[0], v[1]);
    }

    std::cout << g.MaximumMatching() << std::endl;

    return 0;
}
