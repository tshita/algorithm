[🏠 Home](../../README.md)

Maximum Flow Problem (Capacity Scaling Algorithm)  
最大流問題（容量スケーリング法）
============================================================================================

# **最大流問題**
- Input: 有向グラフ $G = (V, A)$ , 容量 $c : A \rightarrow \mathbb{Z}_{> 0}, s, t \in V$
- Output: $s$ から $t$ へ至る流れで，その値が最大のもの

# Algorithm
最大流問題に対する容量スケーリング法は Harold N. Gabow ('85) によって考案された増加道法の一種で弱多項式時間アルゴリズムである。ただし、容量が整数である必要がある。

> 一般的に、スケーリング法といったら、数値を見る「目盛り」を順に変えることで最適解に「近い」解を順に得ていく手法 [1]

Gabow の容量スケーリング法では、 $U = \max_{a \in A} c(a)$ , $K = \lfloor 1 + \log_2 {U} \rfloor$ , $s \text{-} t$ 流 $f_0 = 0$ として、反復  $k = 1, 2, \ldots, K$ に対して順に以下を実行する。

1. 各 $a \in A$ に対して $u_k(a) = \lfloor u(a) / 2^{K - k} \rfloor$ とする
2. $2 f_{k - 1}$ を初期 $s \text{-} t$ 流として、増加道法でネットワーク $(G, u_k)$ の最大 $s \text{-} t$ 流 $f_k$ を見つける

$f_K$ が求める最大流である。

$K$ は入力グラフの容量を表すのに必要な最小ビット数である。また、 $u_k(a) = \lfloor u(a) / 2^{K - k} \rfloor$ の右辺は弧 $a$ の容量を $k$ ビット右シフトした値となる。すなわち、二進数で幅が $K$ として見たとき上位 $k$ ビットを選ぶことに対応している。  
例えば、容量が $1, 3, 5$ のとき $K = 3$ で二進表記すると $(001)_{2}, (011)_{2}, (101)_{2}$ となる。このとき、

- $k = 1$ ： $(001)_{2} \rightarrow (0)_{2} = (0)_{10}, (011)_{2} \rightarrow (0)_{2} = (0)_{10}, (101)_{2} \rightarrow (1)_{2} = (1)_{10}$
- $k = 2$ ： $(001)_{2} \rightarrow (00)_{2} = (0)_{10}, (011)_{2} \rightarrow (01)_{2} = (1)_{10}, (101)_{2} \rightarrow (10)_{2} = (3)_{10}$
- $k = 3$ ： $(001)_{2} \rightarrow (001)_{2} = (1)_{10}, (011)_{2} \rightarrow (011)_{2} = (3)_{10}, (101)_{2} \rightarrow (101)_{2} = (5)_{10}$

となる。各反復で容量の目盛りが荒い方から細かい方へ変わることが分かる。各反復の制限された容量に対して、前の反復で得られた最大流を初期解として最大流問題を求める。ここで、部分問題としての最大流問題を求めるアルゴリズムは [Ford-Fulkerson method](../../graph/maximum_flow_ford_fulkerson.cc) を使用した。容量の整数性から停止することが保証されていることと実装が簡単だからである。

計算時間は、 各反復での流量の増加分が $m$ 以下であることが知られており、Ford-Fulkerson method の計算時間 $O(m U)$ から $O(m^2)$ となり、反復回数 $K = O(\log U)$ なので全体で $O(m^2  \log U)$ となる。

## Complexity
 $n = |V|, m = |A|, U = \max_{a \in A} c(a)$
- Time: $O(m^2 \log U)$ （弱多項式時間）
- Space: $O(n + m)$

# References
- [1] [岡本吉央：2023年度離散最適化基礎論 ネットワークフロー 第6回「最大流問題：容量スケーリング法」](http://dopal.cs.uec.ac.jp/okamotoy/lect/2023/networkflow/networkflowlect06.pdf)

# Source Code
容量の型は非負整数のみであることに注意。

## Usage
- `CapacityScalingAlgorithm<T> g(n, inf)` : 弧重みの型 `T`, 頂点数 `n`, 容量無限大の値が `inf` の有向グラフを構築（`T` は unsigned integer に制限）
- `CapacityScalingAlgorithm<T> g(n)`: 弧重みの型 `T`，頂点数 `n` の有向グラフを構築（容量無限大は T の最大値 / 10 に設定。`T` は unsigned integer に制限）
- `g.add_arc(u, v, c)` : 容量 `c` の弧 $(u, v)$ を追加
- `g.add_edge(u, v, c)`: 容量 `c` の辺 $\{u, v \}$ を追加
- `g.MaximumFlow(s, t)`: `s` から `t` へ至る最大流の値

## Verified
- [AOJ Network Flow - Maximum Flow](http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=GRL_6_A)

---------------------------------------------------------------------------------------------

<details>
<summary>src/graph/maximum_flow_capacity_scaling.cc を表示</summary>

```cpp
#include <iostream>
#include <vector>
#include <limits>
#include <cmath>

// ------------------8<------- start of solution -------8<------------------------
template<typename Weight>
struct CapacityScalingAlgorithm {
    static_assert(std::is_unsigned<Weight>::value, "Nonnegative integer required.");

    struct Edge {
        int src, dst; // 弧の始点、終点
        int rev;      // 補助ネットワークの隣接リストにおける逆弧の添え字

        const Weight cap;       // 入力グラフの弧容量
        Weight cap_aux = 0;     // 補助ネットワークにおける弧容量
        const bool is_original; // 入力グラフの弧

        Edge(int s, int t, Weight c, int r, bool is_org) :
            src(s), dst(t), rev(r), cap(c), is_original(is_org) {}
    };

    int n;                              // 頂点数
    std::vector<std::vector<Edge>> adj; // 入力グラフの隣接リスト
    const Weight INF;                   // 最大流よりも大きな値
    unsigned NUM_ITER;                  // 反復回数

    explicit CapacityScalingAlgorithm(int _n, Weight inf = std::numeric_limits<Weight>::max() / 10)
        : n(_n), adj(n), INF(inf) {}

    void add_arc(const int src, const int dst, const Weight cap) {
        adj[src].emplace_back(Edge(src, dst, cap, adj[dst].size(), true));
        adj[dst].emplace_back(Edge(dst, src, cap, adj[src].size() - 1, false));
    }

    void add_edge(const int src, const int dst, const Weight cap) {
        add_arc(src, dst, cap);
        add_arc(dst, src, cap);
    }

    // s-t 最大流の値を求める
    Weight MaximumFlow(const int s, const int t) {
        // 反復回数を計算
        Weight max_cap = 0;
        for (int v = 0; v < n; ++v) {
            for (const auto &e: adj[v]) {
                if (e.is_original) max_cap = std::max(max_cap, e.cap);
            }
        }
        NUM_ITER = std::floor(std::log2(max_cap) + 1);

        // 容量スケーリング法で最大流を計算
        Weight flow = 0;
        for (unsigned k_th = 1; k_th <= NUM_ITER; ++k_th) {
            DoubleFlowAndMakeAuxiliaryNetwork(k_th);

            flow *= 2;
            while (true) {
                std::vector<bool> visited(n, false);
                Weight f = Dfs(s, INF, t, visited);
                if (f <= 0) break;
                flow += f;
            }
        }

        return flow;
    }
    
    // 現在の流量を二倍にして、各弧容量のスケーリングを変えた補助ネットワークを作成
    void DoubleFlowAndMakeAuxiliaryNetwork(const unsigned k) {
        // std::cout << "DoubleFlowAndAuxiliaryNetwork" << std::endl;
        for (int v = 0; v < n; ++v) {
            for (auto &&e: adj[v]) {
                // 補助ネットワークの逆弧の容量が流量に等しいので変更の基準とする
                if (!e.is_original) {
                    // k_th 反復における補助ネットワークの容量
                    const Weight cap_aux = e.cap >> (NUM_ITER - k);

                    e.cap_aux *= 2;
                    adj[e.dst][e.rev].cap_aux = cap_aux - e.cap_aux;
                }
            }
        }
    }

    // 深さ優先探索で現在の補助ネットワークにおける増加道を見つける
    Weight Dfs(int v, Weight f, const int t, std::vector<bool> &visited) {
        if (v == t) return f;

        visited[v] = true;
        for (auto &&e : adj[v]) {
            if (!visited[e.dst] && 0 < e.cap_aux) {
                Weight d = Dfs(e.dst, std::min(f, e.cap_aux), t, visited);
                if (0 < d) {
                    e.cap_aux -= d;
                    adj[e.dst][e.rev].cap_aux += d;
                    return d;
                }
            }
        }
        return 0;
    }
};
// ------------------8<------- end of solution ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    int n, m;
    std::cin >> n >> m;

    CapacityScalingAlgorithm<unsigned long long> solver(n);
    for (int i = 0, u, v, c; i < m; ++i) {
        std::cin >> u >> v >> c;
        solver.add_arc(u, v, c);
    }
    std::cout << solver.MaximumFlow(0, n - 1) << std::endl;

    return 0;
}
```

</details>
