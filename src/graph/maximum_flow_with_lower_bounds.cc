/*
  Maximum flow with lower bounds problem
  容量下界制約付き最大流問題
  =========================================

  # Problem
    Input: 有向グラフ G = (V, A), 容量 c : A -> R, 容量下界 lb : A -> R, s, t \in V
    Output: s から t へ至る流れ f で，その値が最大のもの
    　　　　　（ただし，どの辺 e に対しても lb(e) <= f(e) <= c(e) を満たす）

  # Complexity (n = |V|, m = |A|)
    - Time: O(m n^2)
    - Space: O(n + m)

  # Usage
    - using algo = Algorithm<T>: 弧重みの型 T の最大流問題を解くアルゴリズム（Dinic's algorithm を使用）
    - MaximumFlowWithLowerBound<algo> g(n): 頂点数 n の有向グラフを構築
    - MaximumFlowWithLowerBound<algo> g(n, inf): 頂点数 n, 容量無限大の値が inf の有向グラフを構築
    - g.add_arc(u, v, lb, ub): 弧 (u, v) とその容量制約の下界 lb と上界 ub を追加
    - g.MaximumFlow(s, t): s から t へ至る容量下限制約付き最大流の値

  # Description
  　下界制約付き最大流問題は super source S と super sink T を追加して最大流問題へと帰着する．

  # Note
    - Ford-Fulkerson method を使うとエラーが発生するので Dinic's algorithm を使用
    - 弧重みが浮動小数点数のときは，MaximumFlowメソッド内の != を適切なものに変更する必要あり
    - 問題名の和訳が正しいのか不明

  # References
    - [@snuke 最小流量制限付き最大流](https://snuke.hatenablog.com/entry/2016/07/10/043918)
    - [@tubo28 流量の下限制約付き最大流](https://tubo28.me/algorithm/flow_with_lu_bound/)
    - 浅野孝夫：『グラフ・ネットワークアルゴリズムの基礎 数理とCプログラム』．近代科学社，2017，pp. 158--161

  # Verified
    - [AOJ1615 Gift Exchange Party]
      (http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=1615)
*/

#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <cassert>


//  @require ./maximum_flow_dinic.cc 👇👇

// -------------------8<------- start of library -------8<------------------------
template<class Algorithm>
struct MaximumFlowWithLowerBound {
    using Weight = typename Algorithm::weight_type;

    Algorithm algo;
    const int SuperSource, SuperSink;
    Weight sum_lb = 0;

    MaximumFlowWithLowerBound(int n)
        : algo(n + 2), SuperSource(n), SuperSink(n + 1) {}

    MaximumFlowWithLowerBound(int n, Weight inf)
        : algo(n + 2, inf), SuperSource(n), SuperSink(n + 1) {}

    void add_arc(const int src, const int dst, const int lb, const int ub) {
        assert(0 <= lb && lb <= ub);
        if (src == dst || ub == 0) return ;
        algo.add_arc(src, dst, ub - lb);
        algo.add_arc(SuperSource, dst, lb);
        algo.add_arc(src, SuperSink, lb);
        sum_lb += lb;
    }

    Weight MaximumFlow(const int s, const int t) {
        // Weight が浮動小数点数型なら != に注意
        const Weight f1 = algo.MaximumFlow(SuperSource, SuperSink);
        const Weight f2 = algo.MaximumFlow(s, SuperSink);
        if (f1 + f2 != sum_lb) return -1;

        const Weight f3 = algo.MaximumFlow(SuperSource, t);
        if (f1 + f3 != sum_lb) return -1;

        const Weight f4 = algo.MaximumFlow(s, t);
        return f2 + f4;
    }
};
// ------------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    int n, m;
    std::cin >> n >> m;

    using Weight = int;
    MaximumFlowWithLowerBound<Dinic<Weight>> dinic(n);
    for (int i = 0; i < m; ++i) {
        int src, dst;
        Weight lb, ub;
        std::cin >> src >> dst >> lb >> ub;
        dinic.add_arc(src, dst, lb, ub);
    }

    int Source, Sink;
    std::cin >> Source >> Sink;

    Weight max_f = dinic.MaximumFlow(Source, Sink);
    if (max_f == -1) std::cout << "infeasible\n";
    else std::cout << "Maximum flow is " << max_f << std::endl;

    return 0;
}
