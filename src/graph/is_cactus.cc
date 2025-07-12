/*
  Cactus Recognition Problem
  カクタスの認識問題
  ==========================

  # Problem
    Input: 無向グラフ G = (V, E)
    Output: G が Cactus であるか判定

    Def. Cactus
      無向グラフ G が Cactus であるとは，G が連結で，G のすべての辺が高々1つの単純閉路にしか含まれないこと

  # Complexity (n = |V|, m = |E|)
    - Time: O(n + m)
    - Space: O(n + m)

  # Usage
    - Graph g(n): 頂点数 n の無向グラフを構築
    - g.add_edge(u, v): 辺 {u, v} を追加
    - g.IsCactus(): g が Cactus かどうかを判定する

  # Description
  　Cactus は外平面的グラフであり，外平面的グラフならば |E| <= 2 * |V| - 3 を満たすので，
  　まず初めに，|E| <= 2 * |V| - 3 かどうかを判定して，満たさないならば Cactus ではないと判定する．

  　次に，G の Dfs木 T を求める．G と T の頂点数が異なれば，G は非連結なので Cactus ではない．
  　ここで，G は連結グラフと仮定する．T に含まれない G の各辺 {u, v} を考える．
  　T に {u, v} を加えると，T は G の全域木なので単純閉路 C ができる．C は u と v の最小共通祖先を
  　p としたとき，u から p への道 P1 と，p から v への道 P2 として，C = P1 P2 u となる．
  　一般的に，最小共通祖先は前処理 O(n log n) 時間，各クエリ O(log n) 時間で求めることができるが，
  　T は Dfs木であり，Dfs木に含まれないすべての辺は後退辺（backward edge）となる性質から，
  　u と v の最小共通祖先は u または v の高さが低い方なので，前処理 O(n) 時間，
  　各クエリ O(1) 時間で求めることができるができる．
  　T に含まれない各辺 {u, v} を加えてできる単純閉路を塗ることを考えると，G が Cactus であるための
  　必要十分条件は，T の各辺が高々1度しか塗られないことである．
  　したがって，|E| <= 2 * |V| - 3 であり，T の各辺は高々1度しか塗らないので，
  　この問題は O(n) 時間で判定可能である．

  　その他にも，G を2点連結成分分解して，各2点連結成分が単純閉路か道となっているならば，
  　G が Cactus であるという判定方法もある．上で説明したアルゴリズムは本質的には2点連結成分分解と同じである

  # Verified
    - [POJ2793 Cactus](http://poj.org/problem?id=2793)
*/

#include <iostream>
#include <vector>

// -------------8<------- start of library -------8<------------------------
struct Graph {
    int n, m;
    std::vector<std::vector<int>> adj;
    explicit Graph(int _n) : n(_n), m(0), adj(n) {}
    void add_edge(int u, int v) { ++m; adj[u].push_back(v); adj[v].push_back(u); }

    int Dfs(int pre, int cur, int h) {
        height[cur] = h;
        int num = 1;
        for (const int nxt : adj[cur]) {
            if (0 <= height[nxt] && nxt != pre && cur < nxt)
                backwad_edges.emplace_back(std::make_pair(cur, nxt));
            else if (height[nxt] == -1) {
                tree[nxt] = std::make_pair(cur, -1);
                num += Dfs(cur, nxt, h + 1);
            }
        }
        return num;
    }

    std::vector<int> height;
    std::vector<std::pair<int, int>> tree, backwad_edges;
    bool IsCactus() {
        if (2 * n - 3 < m) return false;

        tree.resize(n); height.resize(n, -1);
        backwad_edges.reserve(std::max(0, m - n + 1));

        if (Dfs(-1, 0, 0) < n) return false;

        for (auto &&[src, dst] : backwad_edges) {
            if (height[src] < height[dst]) std::swap(src, dst);
            for (int cur = src; cur != dst; cur = tree[cur].first) {
                if (tree[cur].second != -1) return false;
                tree[cur].second = true;
            }
        }

        return true;
    }
};
// -------------8<------- end of library ---------8-------------------------

int main() {
    int n, m, v[2];
    std::cin >> n >> m;

    Graph g(n);
    for (int i = 0; i < m; ++i) {
        std::cin >> v[0] >> v[1];
        g.add_edge(v[0], v[1]);
    }

    std::cout << (g.IsCactus() ? "Yes " : "No ") << "Cactus\n";

    return 0;
}
