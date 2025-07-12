/*
  Eulerian Digraph (Hierholzer's Algorithm)
  オイラー有向グラフのハイヤーホルザのアルゴリズム
  ========================================

  # Problem
    Input: 多重有向グラフ G = (V, A)
    Output: G がオイラーグラフか準オイラーグラフかを判定して，存在するならばオイラー路かオイラー閉路を求める

    Def. オイラー路 (Eulerian path)
      G のオイラー路とは，G のすべての弧をちょうど1回通る有向道

    Def. 準オイラーグラフ (semi-Eulerian graph)
      G にオイラー路が存在するとき，G をオイラーグラフと呼ぶ

    Def. オイラー閉路 (Eulerian cycle)
      G のオイラー閉路とは，G のすべての弧をちょうど1回通る有向閉路

    Def. オイラーグラフ (Eulerian graph)
      G にオイラー閉路が存在するとき，G をオイラーグラフと呼ぶ

  # Complexity (n = |V|, m = |A|)
    - Time: O(n + m)
    - Space: O(n + m)

  # Usage
    - Graph g(n): 頂点数 n の有向グラフを構築
    - g.add_edge(u, v): g に弧 (u, v) を追加
    - g.IsEulerianDigraph(): g がオイラーグラフか準オイラーグラフかどうかを判定する(隣接リストを破壊)
    - g.path: g の オイラー路かオイラー閉路の訪れた頂点の列

  # Description
    下の連結性についての条件は，G の基礎グラフでの孤立点は取り除いたものを考える．

    Thm. オイラーグラフ
      有向グラフ G がオイラーグラフであるための必要十分条件は，G が強連結で，かつ，すべての頂点の入次数と
      出次数が等しいことである．

    Thm. 準オイラーグラフ
      有向グラフ G が準オイラーグラフであるための必要十分条件は，出次数が入次数より1大きい頂点 s が
      一意に存在して，入次数が出次数より 1 大きい頂点 t も一意に存在して，それ以外の頂点の入次数と
      出次数が等しく，G の基礎グラフが連結であることである．

    初めに，入次数と出次数の条件を調べる．次に，Hierholzer のアルゴリズムで部分オイラー路または
    部分オイラー閉路を構築する．連結性の条件を満たすとき，このアルゴリズムは次数の条件によって
    オイラー路またはオイラー閉路となる．

    ここでは，オイラーグラフの十分条件を弧数による帰納法で示す．
    G の任意の頂点 s から未訪問の弧が無くなるまで隣接頂点を訪れることを繰り返す．
    このとき，この経路を C とすると，C は s から s への閉路となる．
    なぜならば，s 以外の頂点は訪れたとき，未訪問の弧に制限した出次数が入次数よりも 1 多くなるからである．
    C がオイラー閉路ならば証明が終わりなので，オイラー閉路でないとして仮定して証明を行う．
    ここで，閉路 C を s から順番に訪れながら G のオイラー閉路 C' を構築する．
    初め，C' = C とする．現在，隣接する未訪問の弧が存在するような頂点 v にいるとする．
    ここで， G[A \setminus C] の各頂点の入次数と出次数は等しく強連結で，
    |A \setminus C| < |A| なので帰納法の仮定から v から訪れることができる，未訪問の弧からなる
    オイラー閉路 C_v が存在する．ここで，C' の頂点 v の位置に C_v を追加する．
    C のすべての頂点を訪問した後，C' は G のオイラー閉路となる．
    なぜならば，G は強連結なので G[A \setminus C] は強連結成分に分割されており，各連結成分に対して
    ある C の頂点を含むからである．

    上の証明を深さ優先探索で実装したのが Hierholzer のアルゴリズムである．
    準オイラーグラフも同様の議論で証明可能である．

  # Note
    グラフの隣接リストを破壊するので注意が必要

  # References
    + [Learning Algorithms](http://www.learning-algorithms.com/entry/2017/12/07/193238)
    + [GeeksforGeeks](https://www.geeksforgeeks.org/hierholzers-algorithm-directed-graph/)

  # Verified
    + Eulerian cycle
      - [AOJ 0225](http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=0225)
    + Eulerian cycle and Eulerian path
      - [POJ 1386](http://poj.org/problem?id=1386)
*/

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>

// -------------8<------- start of library -------8<------------------------
struct Graph {
    const int n;
    size_t m = 0, start = 0;
    std::vector<std::vector<int>> adj;
    std::vector<int> deg;

    explicit Graph(int _n) : n(_n), adj(_n), deg(_n) {}

    void add_edge(int src, int dst) {
        ++deg[src]; --deg[dst];
        adj[src].push_back(dst); ++m;
        start = src;
    }

    std::vector<int> path;
    bool IsEulerianDigraph() {
        int num_s = 0, num_t = 0;
        for (int v = 0; v < n; ++v) {
            if (1 < abs(deg[v])) return false;
            else if (deg[v] == -1) ++num_t;
            else if (deg[v] == 1) { ++num_s; start = v; }
        }
        if (!(num_s < 2 && num_s == num_t)) return false;

        path.reserve(m + 1);
        std::function<void(int)> dfs = [&](int cur) {
            while (!adj[cur].empty()) {
                const int nxt = adj[cur].back();
                adj[cur].pop_back();
                dfs(nxt);
            }
            path.push_back(cur);
        };

        dfs(start);
        std::reverse(path.begin(), path.end());
        return (path.size() == m + 1);
    }
};
// -------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    int n, m, v[2];
    std::cin >> n >> m;

    Graph g(n);
    for (int i = 0; i < m; ++i) {
        std::cin >> v[0] >> v[1];
        g.add_edge(v[0], v[1]);
    }

    if (g.IsEulerianDigraph()) {
        std::cout << "Eulerian digraph\n";
        for (auto v : g.path) std::cout << v << " ";
        std::cout << std::endl;
    }
    else std::cout << "Not Eulerian digraph.\n";

    return 0;
}
