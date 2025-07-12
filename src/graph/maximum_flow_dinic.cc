/*
  Maximum flow problem (Dinic's algorithm)
  最大流問題（ディニッツのアルゴリズム）
  ===============================================

  # Problem
    Input: 有向グラフ G = (V, A), 容量 c : A -> R, s, t \in V
    Output: s から t へ至る流れで，その値が最大のもの

  # Complexity (n = |V|, m = |A|)
    - Time: O(m n^2)
    - Space: O(n + m)

  # Usage
    - Dinic<T> g(n, inf): 弧重みの型 T, 頂点数 n, 容量無限大の値が inf の有向グラフを構築
    - Dinic<T> g(n): 弧重みの型 T，頂点数 n の有向グラフを構築（容量無限大は T の最大値 / 10 に設定）
    - g.add_arc(u, v, c): 容量 c の弧 (u, v) を追加
    - g.add_edge(u, v, c): 容量 c の辺 {u, v} を追加
    - g.MaximumFlow(s, t): s から t へ至る最大流の値

  # Description
  　現在の流れ f の補助ネットワークに対して入口 s から幅優先探索を行う．s から頂点 v への距離（弧数）を
  　level(v) としたとき，level(v) = level(u) + 1 となる弧 (u, v) のみに制限した補助ネットワーク
  　を考える．この補助ネットワークはレベルグラフと呼ばれている．レベルグラフ上で s から t への極大フロー f'''
  　を求めて f += f'' として繰り返す．
  　レベルグラフから f''(e) = cap_f(e) となる辺 e を除去したときに s から t への道が存在しないときに，
  　f'' を極大フローと呼ぶ．極大フローはブロッキングフロートも呼ばれている．

  　ディニッツのアルゴリズムはフォード・ファルカーソン法と異なり強多項式時間アルゴリズムである．

  # Note
    - g.INF の値を問題ごとに適切に割り当てないとオーバーフローする可能性がある
    - 容量がすべて1ならば計算時間は O(min{n^{2/3}, m^{1/2}} m)
    - 無向辺を加えるときは，両方向に容量 c の弧を加える（add_edge を使用）

  # References
    - あり本. pp. 188--195
    - [保坂和宏，ネットワークフロー入門](http://hos.ac/slides/20150319_flow.pdf)
    - 浅野孝夫：『グラフ・ネットワークアルゴリズムの基礎 数理とCプログラム』．近代科学社，2017，pp. 126--161

  # Verified
    - [AOJ Network Flow - Maximum Flow]
      (http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=GRL_6_A)
    - [SPOJ FASTFLOW - Fast Maximum Flow](https://www.spoj.com/problems/FASTFLOW/)
*/

#include <iostream>
#include <vector>
#include <queue>
#include <limits>

// -------------------8<------- start of library -------8<------------------------
template<typename Weight>
struct Dinic {
    using weight_type = Weight;

    struct Edge {
        int src, dst, rev;
        Weight weight;
        Edge(int f, int t, Weight cap, int rev = 0) :
            src(f), dst(t), rev(rev), weight(cap) {}
    };

    int n;
    std::vector<std::vector<Edge>> adj;
    const Weight INF;

    explicit Dinic(int _n, Weight inf = std::numeric_limits<Weight>::max() / 10)
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
        while(true) {
            std::vector<int> level(n, -1);
            LevelGraph(s, level);
            if(level[t] == -1) break;

            std::vector<size_t> iter(n);
            Weight blocking_flow = 0;
            while ((blocking_flow = AugmentingPath(s, INF, t, level, iter)) > 0)
                flow += blocking_flow;
        }

        return flow;
    }

    void LevelGraph(const int s, std::vector<int> &level){
        std::queue<int> que;
        for (level[s] = 0, que.push(s); !que.empty(); ) {
            const int v = que.front(); que.pop();
            for (const auto &e : adj[v])
                if(0 < e.weight && level[e.dst] == -1){
                    level[e.dst] = level[v] + 1;
                    que.push(e.dst);
                }
        }
    }

    Weight AugmentingPath(int v, Weight flow, const int t,
                          std::vector<int> &level, std::vector<size_t> &iter) {
        if(v == t) return flow;
        for ( ; iter[v] < adj[v].size(); ++iter[v]) {
            Edge &e = adj[v][iter[v]];

            if(0 < e.weight && level[v] < level[e.dst]){
                Weight d = AugmentingPath(e.dst, std::min(flow, e.weight), t, level, iter);
                if(0 < d){
                    e.weight -= d;
                    adj[e.dst][e.rev].weight += d;
                    return d;
                }
            }
        }
        return 0;
    }
};
// ------------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    int n, m;
    std::cin >> n >> m;

    Dinic<long long> dinic(n);
    for (int i = 0, u, v, c; i < m; ++i) {
        std::cin >> u >> v >> c;
        dinic.add_arc(u, v, c);
    }

    std::cout << dinic.MaximumFlow(0, n - 1) << std::endl;

    return 0;
}
