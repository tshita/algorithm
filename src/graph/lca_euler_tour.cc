/*
  Lowest Common Ancestor by euler tour
  最小共通祖先（オイラーツアー）
  ====================================

  # Problem
    Input: 根付き木 (T = (V, E), r)
    Query: T の頂点 u, v --> u と v の最小共通祖先

    Def. 頂点 v の深さ(Depth)
      根 r から v へ至る道の辺数

    Def. 最小共通祖先(Lowest Common Ancestor)
      u と v の最小共通祖先とは，u と v の共通の祖先(common ancestor)で深さが最も大きい頂点

  # Complexity (n = |V|)
    Time:
      + 前処理: O(n)
      + クエリ: O(log n)
    Space: O(n)

  # Usage
    - LowestCommonAncestor lca(n, r): 頂点数 n, 根のラベル r の根付き木を構成
    - lca.add_edge(u, v): 辺 {u, v} を追加
    - lca.Preprocessing(): クエリに対する前処理
    - lca.query(u, v): u と v の最小共通祖先を求める

  # Description
    まず初めに，木 T のオイラーツアー O を求める．オイラーツアーとは頂点の列で，
    根から Dfs をしたときに，preorder と postorder のそれぞれで頂点を記録した列である．
    すなわち，Dfs で頂点から入るときに記録して，その頂点から出るときにその親を記録した列である．
    オイラーツアーという名前は，木の各辺を行き帰りの2つの弧からなる有向グラフとしたときに，
    その有向グラフ上のオイラー閉路（Euler Tour）に対応することから名付けられた．

    各頂点 v に対して，O 上で v が現れる最も左の添字番号を idx(v) とする．
    このとき，頂点 u, v (idx(u) < idx(v)) の最小共通祖先は O 上の区間 [idx(u), idx(v)]
    に含まれる頂点の中で深さが最小となる頂点である．
    これは u と v の最小共通祖先 p が区間 [idx(u), idx(v)] に含まれており，
    [idx(u), idx(v)] に対応する部分木が p の部分木に等しいことから分かる．

    このクエリは区間の最小値を求める Range Minimum Query (RMQ) に等しいので，
    オイラーツアーで T を列で表した後に，各クエリに対して Segment Tree で RMQ を求める．
    Segment Tree の値はオイラーツアーの頂点とその深さの組でありボトムアップで求めている．
    オイラーツアーの長さは 2n - 1 であり，Segment Tree のサイズもその定数倍にしかならないので，
    前処理は O(n) 時間で求めることができる．

  # References
    - あり本, pp. 294--295
    - [Spaceships解説 @Masaki Hara](https://www.slideshare.net/qnighy/2013-spaceships2)

  # Verified
    - [AOJ GRL_5_C Tree - Lowest Common Ancestor]
      (http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=GRL_5_C)
*/

#include <iostream>
#include <vector>
#include <climits>

// -------------8<------- start of library -------8<------------------------
struct LowestCommonAncestor {
    const int root, n, INF;
    int size, no;
    std::vector<std::vector<int>> adj;
    std::vector<int> idx;
    std::vector<std::pair<int, int>> dep;

    explicit LowestCommonAncestor(int _n, int _r = 0) :
        root(_r), n(_n), INF(INT_MAX), adj(n), idx(n) {}

    void add_edge(int u, int v) { adj[u].push_back(v); adj[v].push_back(u); }

    void Dfs(int cur, int p, int d) {
        idx[cur] = no;
        dep[size - 1 + no++] = std::make_pair(d, cur);
        for (int nxt : adj[cur]) {
            if (nxt != p) {
                Dfs(nxt, cur, d + 1);
                dep[size - 1 + no++] = std::make_pair(d, cur);
            }
        }
    }

    void Preprocessing() {
        for (size = 1; size < 2 * n - 1; ) size <<= 1;
        dep.assign(2 * size - 1, std::make_pair(INF, INF));

        no = 0;
        Dfs(root, -1, 0);

        for (int i = size - 2; 0 <= i; --i)
            dep[i] = std::min(dep[2 * i + 1], dep[2 * i + 2]);
    }

    int rmq(int l, int r) {
        std::pair<int, int> res(INF, INF);
        l += size - 1;
        r += size - 1;
        while (l < r) {
            if ((l & 1) == 0) res = std::min(res, dep[l]);
            if ((r & 1) == 0) res = std::min(res, dep[r - 1]);
            l >>= 1;
            r = (r - 1) >> 1;
        }
        return res.second;
    }

    int query(int u, int v) {
        if (idx[v] < idx[u]) std::swap(u, v);
        return rmq(idx[u], idx[v] + 1);
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
