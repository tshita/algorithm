/*
  Lowest Common Ancestor by doubling
  最小共通祖先（ダブリング）
  ==================================

  # Problem
    Input: 根付き木 (T = (V, E), r)
    Query: u, v \in V --> u と v の最小共通祖先

    Def. 頂点 v の深さ(Depth)
      根 r から v へ至る道の辺数

    Def. 最小共通祖先(Lowest Common Ancestor)
      u と v の最小共通祖先とは，u と v の共通の祖先(common ancestor)で深さが最も深い頂点

  # Complexity (n = |V|)
    Time:
      + 前処理: O(n log n)
      + クエリ: O(log n)
    Space: O(n log n)

  # Usage
    - LowestCommonAncestor lca(n, r): 頂点数 n, 根のラベル r の根付き木を構成
    - lca.add_edge(u, v): 辺 {u, v} を追加
    - lca.Preprocessing(): クエリに対する前処理
    - lca.query(u, v): u と v の最小共通祖先を求める

    - lca.dep[v]: v の深さ
    - lca.par[k][v]: v から根へ辺をちょうど 2^k 回辿ることによって到達可能な祖先 (存在しない場合は -1)

  # Description
    頂点 v の深さを d(v) としたとき，v から根へ親を d(v) 回辿ることによって根に到達することができる．
    また，d(v) >= d(u) とすると，初めに v から根へ親を d(v) - d(u) 回辿り，次に u と v を
    同時に1回ずつ共通の頂点に到達するまでそれぞれの親を辿ることによって最小共通祖先に到達することができる．
    上の方法では各クエリで O(n) となるので，ダブリングという手法を用いて高速化する．
    f(k, v) を上の Usageの lca.par[k][v] とすると，f(k + 1, v) = f(k, p(k, v)) となる．
    また，k は高々 O(log n) なので，f(k, v) は O(n log n) で計算可能である．
    上の説明の， v から根へ親を d(v) - d(u) 回辿ることは，冪乗法のようにすると O(log n) 回で到達できる．
    また，最小共通祖先からは何回親を辿っても共通の頂点となるので，u と v から同時に1回ずつ親を辿る方法も，
    冪乗法のようにして O(log n) 回で最小共通祖先へ到達できる．

  # References
    - あり本, pp. 292--295
    - [@tubo28](https://tubo28.me/algorithm/doubling-lca/)
    - [@tmaehara](https://github.com/spaghetti-source/algorithm/blob/master/graph/least_common_ancestor_doubling.cc)

  # Verified
    - [AOJ GRL_5_C Tree - Lowest Common Ancestor]
      (http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=GRL_5_C)
*/

#include <iostream>
#include <vector>
#include <cmath>

// -------------8<------- start of library -------8<------------------------
struct LowestCommonAncestor {
    const int root, UB;
    int n;
    std::vector<std::vector<int>> adj, par;
    std::vector<int> dep;

    explicit LowestCommonAncestor(int _n, int _r = 0) :
        root(_r), UB(log2(_n) + 1), n(_n), adj(n),
        par(UB, std::vector<int>(n)), dep(n) {}

    void add_edge(int u, int v) { adj[u].push_back(v); adj[v].push_back(u); }

    void Preprocessing() {
        Dfs(root, -1, 0);
        for (int k = 0; k + 1 < UB; ++k)
            for (int v = 0; v < n; ++v)
                par[k + 1][v] = (par[k][v] < 0 ? -1 : par[k][par[k][v]]);
    }

    void Dfs(int cur, int p, int d) {
        par[0][cur] = p;
        dep[cur] = d;
        for (const int dst : adj[cur])
            if (dst != p) Dfs(dst, cur, d + 1);
    }

    int query(int u, int v) {
        if (dep[u] > dep[v]) std::swap(u, v);
        for (int k = 0; k < UB; ++k)
            if ((dep[v] - dep[u]) >> k & 1) v = par[k][v];
        if (u == v) return u;
        for (int k = UB - 1; 0 <= k; --k)
            if (par[k][u] != par[k][v]) { u = par[k][u]; v = par[k][v]; }
        return par[0][u];
    }
};
// -------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    // AOJ GRL_5_C: Tree - Lowest Common Ancestor
    int n;
    std::cin >> n;

    LowestCommonAncestor lca(n, 0);
    for (int v = 0, deg, c; v < n; ++v) {
        std::cin >> deg;
        while (deg--) {
            std::cin >> c;
            lca.add_edge(v, c);
        }
    }
    lca.Preprocessing();

    int q, u, v;
    std::cin >> q;
    while (q--) {
        std::cin >> u >> v;
        std::cout << lca.query(u, v) << '\n';
    }

    return 0;
}
