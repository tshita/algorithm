/*
  Maximum Flow Problem (Ford-Fulkerson method)
  最大流問題（フォード・ファルカーソン法）
  ===============================================

  # Problem
    Input: 有向グラフ G = (V, A), 容量 c : A -> R, s, t \in V
    Output: s から t へ至る流れで，その値が最大のもの

  # Complexity (n = |V|, m = |A|, f = 最大流の値, U = 弧容量の最大値)
    - Time: O(m f) または O(m^2 U) （f <= m U）
    - Space: O(n + m)

  # Usage
    - FordFulkerson<T> g(n, inf): 弧重みの型 T, 頂点数 n, 容量無限大の値が inf の有向グラフを構築
    - FordFulkerson<T> g(n): 弧重みの型 T，頂点数 n の有向グラフを構築（容量無限大は T の最大値 / 10 に設定）
    - g.add_arc(u, v, c): 容量 c の弧 (u, v) を追加
    - g.add_edge(u, v, c): 容量 c の辺 {u, v} を追加
    - g.MaximumFlow(s, t): s から t へ至る最大流の値

  # Description
  　Ford-Furkersonのアルゴリズムは増加道法の選び方に工夫のないアルゴリズム．
  　補助ネットワークにおいて，s を始点，t を終点とする道を増加道（ぞうかどう）と呼ぶ．
  　補助ネットワークにおいて増加道を見つけて，その道に沿って流れを増加させる操作を増加道が
  　存在しなくなるまで繰り返す方法を 増加道法（augmenting path method） と呼ぶ．

  # Note
    - g.INF の値を問題ごとに適切に割り当てないとオーバーフローする可能性がある
    - 最大流問題の最適性条件：f が最大 s-t 流 <=> f に対する増加道が存在しない
    - 増加道法が停止するなら最大 s-t 流を出力する（仮定が満たされない例も存在する）
    - 容量に無理数を含まなければ擬多項式時間アルゴリズム
    - 容量に無理数が含まれると Zwick の例のように有限ステップで終了しない場合がある。
    　また，収束先が最大流に等しくない場合もあるので注意（[岡本吉央：増加道法が停止しない例＠YouTube](https://youtu.be/L3YDFbQFM2k)）
    - 無向辺を加えるときは，両方向に容量 c の弧を加える（add_edge を使用）

  # References
    - あり本. pp. 188--195
    - [保坂和宏，ネットワークフロー入門](http://hos.ac/slides/20150319_flow.pdf)
    - [岡本吉央：2023年度離散最適化基礎論「ネットワークフロー」](http://dopal.cs.uec.ac.jp/okamotoy/lect/2023/networkflow/)

  # Verified
    - [AOJ Network Flow - Maximum Flow]
      (http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=GRL_6_A)
*/

#include <iostream>
#include <vector>
#include <limits>

// -------------8<------- start of library -------8<------------------------
template<typename Weight>
struct FordFulkerson {
    using weight_type = Weight;

    struct Edge {
        int src, dst, rev;
        Weight cap;
        Edge(int f, int t, Weight c, int r) :
            src(f), dst(t), rev(r), cap(c) {}
    };

    int n;
    std::vector<std::vector<Edge>> adj;
    const Weight INF;

    explicit FordFulkerson(int _n, Weight inf = std::numeric_limits<Weight>::max() / 10)
        : n(_n), adj(n), INF(inf) {}

    void add_arc(const int src, const int dst, const Weight cap) {
        adj[src].emplace_back(Edge(src, dst, cap, adj[dst].size()));
        adj[dst].emplace_back(Edge(dst, src, 0, adj[src].size() - 1));
    }

    void add_edge(const int src, const int dst, const Weight cap) {
        add_arc(src, dst, cap);
        add_arc(dst, src, cap);
    }

    Weight MaximumFlow(const int s, const int t) {
        Weight flow = 0;
        while (true) {
            std::vector<bool> visited(n, false);
            Weight f = Dfs(s, INF, t, visited);
            if (f <= 0) break;
            flow += f;
        }
        return flow;
    }

    Weight Dfs(int v, Weight f, const int t, std::vector<bool> &visited) {
        if (v == t) return f;

        visited[v] = true;
        for (auto &&e : adj[v]) {
            if (!visited[e.dst] && 0 < e.cap) {
                Weight d = Dfs(e.dst, std::min(f, e.cap), t, visited);
                if (0 < d) {
                    e.cap -= d;
                    adj[e.dst][e.rev].cap += d;
                    return d;
                }
            }
        }
        return 0;
    }
};
// -------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    int n, m;
    std::cin >> n >> m;

    FordFulkerson<long long> ford_fulkerson(n);
    for (int i = 0, u, v, c; i < m; ++i) {
        std::cin >> u >> v >> c;
        ford_fulkerson.add_arc(u, v, c);
    }
    std::cout << ford_fulkerson.MaximumFlow(0, n - 1) << std::endl;

    return 0;
}
