/*
  Maximum Independent Set Problem (using Branch and Reduce)
  最大独立集合問題（Branch and Reduce を使用）
  ==========================================================

  # Problem
    Input: 無向グラフ G =(V, E)
    Output: G の最大独立集合

    Def. 独立集合（independent set）
      頂点部分集合 I \subseteq V で，I のどの異なる2頂点も隣接していないものを G の独立集合と呼ぶ．
      サイズ最大の独立集合を G の最大独立集合と呼ぶ．

  # Complexity (n = |V|, m = |E|)
    - Time: O(n * 1.4423^n)
    - Space: O(n + m)

  # Usage
    - MaximumIndependentSet mis(n): 頂点数 n の無向グラフを構成
    - mis.add_edge(u, v): 辺 {u, v} を加える
    - sol = mis.Solve(): 最大独立集合 sol を求める
      - sol.first: 最大独立集合のサイズ
      - sol.second[v]: 頂点 v が最大独立集合の要素かどうか

  # Description
    頂点 v の隣接頂点集合を N(v) とする．グラフの次数最小の頂点を v とする．
    任意の極大独立集合 I に対して，v \in I または u \in I (u \in N(v)) が成り立つ．
    よって，上の分枝操作を部分問題に対して再帰的に行う．
    ここで，
    　T(n) := 頂点数 n の任意のグラフに対して上の Branch and Reduce を行ったときの列挙木のサイズの上界
    とする．このとき，
    　T(n) <= 1 + T(n - d(v) - 1) + sum_{i=1}^{d(v)} T(n - d(v_i) - 1)
    　     <= 1 + (d(v) + 1) * T(n - d(v) - 1)
    　     <= 1 + s + s^2 + ... + s^{n/s} (s = d(v) + 1)
    　     = O*(s^{n/s})
    となる．s^{n/s} を最大にする s は 3 なので，計算時間は O*(3^{n/3}) となる．

  # References
    - Fomin, Kratsch, "Exact Exponential Algorithms", pp. 7--10.

  # Verified
    - 頂点数25以下のランダムグラフに対して全列挙と比較
    - [CODE THANKS FESTIVAL 2017 G - Mixture Drug]
      (https://code-thanks-festival-2017-open.contest.atcoder.jp/tasks/code_thanks_festival_2017_g)

*/

#include <iostream>
#include <vector>
#include <random>

// -----------------------8<------- start of library -------8<-----------------------------
struct Graph {
    int n;
    std::vector<std::vector<int>> adj;
    explicit Graph(int _n) : n(_n), adj(_n) {}
    void add_edge(const int u, const int v) { adj[u].push_back(v); adj[v].push_back(u); }
};

struct MaximumIndependentSet : public Graph {
    using Solution = std::pair<int, std::vector<bool>>;
    Solution sol, opt_sol;
    std::vector<int> deg, selected;

    explicit MaximumIndependentSet(int _n) :
        Graph(_n), sol(0, std::vector<bool>(_n, false)),
        opt_sol(0, std::vector<bool>(_n, false)), deg(n), selected(n, -1) {}

    Solution Solve() {
        for (int v = 0; v < n; ++v) deg[v] = adj[v].size();
        BranchAndReduce(0);
        return opt_sol;
    }

    void BranchAndReduce(int num_selected) {
        if (num_selected == n) {
            if (opt_sol.first < sol.first) opt_sol = sol;
            return ;
        }

        int min_deg_v = -1;
        for (int v = 0; v < n; ++v)
            if (selected[v] == -1 && (min_deg_v == -1 || deg[v] < deg[min_deg_v]))
                min_deg_v = v;

        BranchAndReduce(num_selected + Select(min_deg_v));
        Undo(min_deg_v);

        if (deg[min_deg_v] != 1) {
            for (int u : adj[min_deg_v]) {
                if (selected[u] != -1) continue;
                BranchAndReduce(num_selected + Select(u));
                Undo(u);
            }
        }
    }

    int Select(const int v) {
        sol.second[v] = true;
        ++sol.first;
        selected[v] = v;
        int num = deg[v] = 0;
        for (const int u : adj[v])
            if (selected[u] == -1) {
                selected[u] = v;
                ++num;
                --deg[u];
            }
        return num + 1;
    }

    void Undo(const int v) {
        sol.second[v] = false;
        --sol.first;
        selected[v] = -1;
        for (const int u : adj[v]) {
            if (selected[u] == v) {
                ++deg[v]; ++deg[u];
                selected[u] = -1;
            }
        }
    }
};
// -----------------------8<------- end of library ---------8------------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    int n, m;
    std::cin >> n >> m;

    MaximumIndependentSet mis(n);
    for (int i = 0, u, v; i < m; ++i) {
        std::cin >> u >> v;
        mis.add_edge(u, v);
    }

    mis.Solve();
    std::cout << "the size of independent set = " << mis.opt_sol.first << std::endl;
    for (int v = 0; v < mis.n; ++v)
        if (mis.opt_sol.second[v]) std::cout << v << " ";
    std::cout << std::endl;

    return 0;
}
