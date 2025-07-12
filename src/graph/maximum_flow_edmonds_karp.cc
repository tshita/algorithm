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
