/*
  2-satisfiability problem (2-SAT problem)
  ========================================

  # Problem
    Input: n 個の変数と m 個の節からなる 2-CNF式 φ
    Output: φ は充足可能かどうか（充足可能ならばその真偽値割当を求める）

    Def. 連言標準形 (CNF; Conjunctive Normal Form)
      命題論理式が複数の節の連言(AND)となっているとき連言標準形と呼ぶ．
      特に，各節に含まれるリテラルの数が高々 k 個のとき，k-CNF式と呼ぶ.
        + リテラル(literal): 命題変数またはその否定
        + 節(clause): 複数のリテラルの選言(OR)
      eg.) φ = (x1 v x2) ^ (!x1 v !x3) ^ (x2)

    Def. 充足可能性問題 (SAT; satisfiability problem)
      命題論理式が与えられたときに，式の値が真となる真偽値割当が存在するかどうかを判定する問題．
      特に，命題論理式がk-CNF式のときこの問題を k-SAT問題 と呼ぶ．
      eg.) φ は充足可能である．その真偽値割当は (x1, x2, x3) = (t, t, f)

  # Complexity
    - Time: O(n + m)
    - Space: O(n + m)

  # Usage
    - TwoSat sat(n): n変数からなる2-CNF式を構築 (x_i は i, !x_i は i + n に番号付け)
    - sat.add_clause(lt1, ng1, lt2, ng2): 節 (lt1 v lt2) を追加（ng1, ng2 は各リテラルの符号）
    - sat.Solve(): 充足可能かどうかを判定する
    - sat.Check(): 充足可能性かどうか（事前に sat.Solve() を行う）
    - sat.val[i]: x_i の真偽値割当（充足可能の場合のみ意味を持つ）

  # Note
    次の論理演算は 2SAT を保ちながら同値変形可能
    　・含意 x => y: !x v y
    　・同値 x <=> y: (!x v y) ^ (x v !y)
    　・二重否定 !!x: x

  # Description
    k-SAT問題 (k >= 3) はNP完全だが，2-SAT問題は線形時間で解くことができる．

    節 (x v y) に対して，(x v y) <=> (!x => y) ^ (!y => x) という同値変形を行う．
    n 変数からなる2-CNF式に対応する有向グラフ G = (V, E) を次のように構成する．
    　・ V := 2-CNF式の各変数とその否定からなる集合
    　・ E := 各節 (x v y) に対して，弧 (!x, y) と (!y, x) からなる集合
    G は Implication graph と呼ばれている．
    リテラル x, y が同じ強連結成分に属しているとき含意の推移性から x <=> y となるので，
    G の各強連結成分に属する頂点に対応するリテラルに対して同じ真偽値割当を行う．
    このとき，ある変数 x に対して，x と !x が同じ強連結成分に属しているとき x <=> !x より矛盾が生じるので
    充足不能となる．充足可能なときにはトポロジカルソート順で x が !x よりも前にあるとき真を割り当てる．

  # References
    - あり本．pp. 288-290.
    - [CP-Algorithms 2-SAT](https://cp-algorithms.com/graph/2SAT.html)

  # Verified
    - [yukicoder No.274 The Wall](https://yukicoder.me/problems/no/274)
    - [POJ3905 Perfect Election](http://poj.org/problem?id=3905)
*/

#include <iostream>
#include <vector>

//  @require ../graph/strongly_connected_component_kosaraju.cc 👇👇

// -------------8<------- start of library -------8<------------------------
struct TwoSat {
    const int n;
    std::vector<bool> val;
    Graph g;

    explicit TwoSat(int _n) : n(_n), val(n + 1), g(2 * n) {}

    void add_clause(int lt1, bool ng1, int lt2, bool ng2) {
        g.add_edge(lt1 + (ng1 ? n : 0), lt2 + (ng2 ? 0 : n));
        g.add_edge(lt2 + (ng2 ? n : 0), lt1 + (ng1 ? 0 : n));
    }

    bool Check() const { return val[n]; }

    bool Solve() {
        g.StronglyConnectedComponents();
        for (int x = 0; x < n; ++x) {
            if (g.scc[x] == g.scc[x + n]) return val[n] = false;
            val[x] = (g.scc[x] > g.scc[x + n]);
        }
        return val[n] = true;
    }
};
// -------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    int n, m;
    std::cin >> n >> m;

    TwoSat sat(n);
    for (int i = 0, lt1, lt2; i < m; ++i) {
        bool ng1, ng2;
        std::cin >> lt1 >> ng1 >> lt2 >> ng2;

        // (lt1 v lt2) : ng1 は lt1 の符号，ng2 は lt2 の符号
        sat.add_clause(lt1, ng1, lt2, ng2);
    }

    std::cout << (sat.Solve() ? "Yes" : "No") << std::endl;

    return 0;
}
