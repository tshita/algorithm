[🏠 Home](../../README.md)

Maximum Flow Problem (Edmonds-Karp algorithm)  
最大流問題（エドモンズ・カープのアルゴリズム）
============================================================================================

# **最大流問題**
- Input: 有向グラフ $G = (V, A)$ , 容量 $c : A \rightarrow R, s, t \in V$
- Output: $s$ から $t$ へ至る流れで，その値が最大のもの

# Algorithm
Jack Edmonds（ジャック・エドモンズ）と Richard Karp（リチャード・カープ）による増加道法の一種で、増加道を補助ネットワークにおける弧長最小の経路として選択する。増加道の選び方に工夫のない手法に [Ford-Fulkerson method](../../graph/maximum_flow_ford_fulkerson.cc) がある。  

増加道は補助ネットワークにおける容量が正の弧のみを使った $s-t$ 経路である。その中で弧長最小の経路は幅優先探索で求まる。幅優先探索木の各頂点についてその親を記憶させることによって $s-t$ 経路を復元する。更新する増加道の流れの増加量は $s-t$ 経路中の容量の最小値で、まずそれを求めて、その後で実際に更新するという 2 つの操作に分けて実装している。補助ネットワークに多重弧が存在する場合もあるが、幅優先探索中に各頂点は多くとも一回しかキューに入れないので、幅優先探索木の各頂点の親が 2 つ以上あることはない。したがって、増加量の更新の 2 つの操作で選ぶ $s-t$ 経路は同じものである。

Ford-Fulkerson method は Zwick の例などのように停止しない場合があるが、Edmonds-Karp algorithm は必ず停止することが保証されている。各ステップにおける増加道の弧の数が単調非減少で、 $i$ 回目と $i + |A|$ 回目の反復で得られる増加道の弧の数が真に増加しており、増加道の弧の数が $|V| - 1$ 以下と上に有界なので必ず停止する。詳しい証明は [1] を参照。


## Complexity ( $n = |V|, m = |A|$)
- Time: $O(n m^2)$
- Space: $O(n + m)$

# References
- [1] [岡本吉央：2023年度離散最適化基礎論 ネットワークフロー 第5回「最大流問題：Edmonds-Karp のアルゴリズム」](http://dopal.cs.uec.ac.jp/okamotoy/lect/2023/networkflow/)

# Source Code
入力グラフの各弧の容量は補助ネットワークにおける容量として扱うので、最大流を求めた後の値は変わる可能性があることに注意。元のグラフの容量が必要な場合は `Edge` 構造体にメンバ変数を追加するなどが必要。

## Usage
- `EdmondsKarp<T> g(n, inf)` : 弧重みの型 `T`, 頂点数 `n`, 容量無限大の値が `inf` の有向グラフを構築
- `EdmondsKarp<T> g(n)`: 弧重みの型 `T`，頂点数 `n` の有向グラフを構築（容量無限大は T の最大値 / 10 に設定）
- `g.add_arc(u, v, c)` : 容量 `c` の弧 $(u, v)$ を追加
- `g.add_edge(u, v, c)`: 容量 `c` の辺 $\{u, v \}$ を追加
- `g.MaximumFlow(s, t)`: `s` から `t` へ至る最大流の値

## Verified
- [AOJ Network Flow - Maximum Flow](http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=GRL_6_A)

---------------------------------------------------------------------------------------------

<details>
<summary>src/graph/maximum_flow_edmonds_karp.cc を表示</summary>

```cpp
#include <iostream>
#include <vector>
#include <limits>
#include <queue>

// -------------8<------- start of solution -------8<------------------------
template<typename Weight>
struct EdmondsKarp {
    using weight_type = Weight;

    struct Edge {
        int src, dst; // 弧の始点、終点
        int rev; // 補助ネットワークにおける対となる逆辺の添え字
        Weight cap; // 補助ネットワークにおける容量
        Edge(int f, int t, Weight c, int r) :
            src(f), dst(t), rev(r), cap(c) {}
    };

    int n;
    std::vector<std::vector<Edge>> adj;
    const Weight INF;

    explicit EdmondsKarp(int _n, Weight inf = std::numeric_limits<Weight>::max() / 10)
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
            Weight f = Bfs(s, t);
            if (f <= 0) break;
            flow += f;
        }
        return flow;
    }

    Weight Bfs(const int s, const int t) {
        std::vector<int> prev(n, -1);
        std::queue<unsigned> que;

        prev[s] = 0;
        que.push(s);

        // 補助ネットワーク上で s から t への弧長最小経路を幅優先探索
        bool reached = false;
        while (!reached && !que.empty()) {
            const int v = que.front(); que.pop();
            for (const auto &e: adj[v]) {
                if (0 >= e.cap) continue; // 弧が存在しない

                if (e.dst == t) {
                    prev[t] = v;
                    reached = true;
                    break;
                }

                if (prev[e.dst] == -1) {
                    prev[e.dst] = v;
                    que.push(e.dst);
                }
            }
        }

        //増加道が存在しない
        if (prev[t] == -1) return 0;
        
        // 更新する増加道における流の増加量を計算
        Weight f = INF;
        for (int v = t; v != s; v = prev[v]) {
            for (const auto &e: adj[prev[v]]) {
                if (e.dst == v) {
                    f = std::min(f, e.cap);
                    break;
                }
            }
        }

        // 増加道の更新
        for (int v = t; v != s; v = prev[v]) {
            for (auto &&e: adj[prev[v]]) {
                if (e.dst == v) {
                    e.cap -= f;
                    adj[e.dst][e.rev].cap += f;
                }
            }
        }

        return f;
    }
};
// -------------8<------- end of solution ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    int n, m;
    std::cin >> n >> m;

    EdmondsKarp<long long> solver(n);
    for (int i = 0, u, v, c; i < m; ++i) {
        std::cin >> u >> v >> c;
        solver.add_arc(u, v, c);
    }
    std::cout << solver.MaximumFlow(0, n - 1) << std::endl;

    return 0;
}
```

</details>
