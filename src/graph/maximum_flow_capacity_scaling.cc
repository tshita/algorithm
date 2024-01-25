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
