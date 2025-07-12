/*********************************************************************************
  Tree Isomorphism Problem (Aho, Hopcroft, Ullman)
  木の同型性判定問題
  ========================================
  Source Code: [Click here](tree_isomorphism.cc)

  # Problem
  **Input**: 木 T, S  
  **Output**: TとSが同型であるか  

  **Def. グラフ同型（isomorphic）**  
  T と S が同型であるとは，全単射 f : V(T) -> V(S) が存在して， 任意の u, v \in V(T) に対して，
  {u, v} \in E(T) <==> {f(u), f(v)} \in E(S)
  が成り立つこと．f を同型写像（isomorphism）と呼ぶ．

  **Def. 根付き木の同型**  
  　根付き木 (T, t) と (S, s) が同型であるとは，同型写像 f : V(T) -> V(S) が存在して，
  　かつ，f(t) = s を満たす．

  **Def. 順序木の同型**  
  　順序木 (T, t, <_T) と (S, s, <_S) が同型であるとは，根付き木の同型写像 f : V(T) -> V(S)
  　が存在して，v \in V(T) に対して，その子全体が c1 <_T c2 <_T ... <_T c_k のとき，
  　f(c_1) <_S f(c_2) <_S ... <_S f(c_k) を満たす．

  # Complexity
  **Time and Space** : O(n)

  # Description
  　木，根付き木，順序木の順番に条件が強くなるので，まず順序木の同型性判定から行う．
  各頂点に二進列をコードとして割り当てる．各葉に 10 を割り当てて，
  頂点 v の子のコードがそれぞれ A_1, A_2, ..., A_t のとき（対応する頂点は線形順序），
  v のコードを 1 A_1 A_2 ... A_t 0 と連結する．  
  　根付き木では A_1, A_2, ..., A_t を連結する順番を決める必要があるが，
  各頂点のコードが二進列であることから自然な順序を定義できて順序木に帰着できる．
  ここの実装をどうするかによって，O(n) か O(n log n) となる．  
  　木の場合は根付き木に帰着するが，根をどうするかが問題となる．ここで，木の不変量である中心を用いる．
  木の頂点 v の離心率とは v から最長の距離で，離心率が最小となる頂点集合が中心である．
  どの木も中心のサイズは高々2なので根の選び方を全通り試す．中心は直径の中央値であるが，
  直径は幅優先探索を2回すると O(n) で求まる．  
  実装方法は2つの木の根から遠いレイヤーごとに行う．上では各頂点に二進列をコードとして割り当てたが，
  ここでは自然数を割り当てる．あるレイヤーの2つの木の頂点全体を考える．
  ここで，各頂点の子に対しては自然数が割り当てられており，その子全体のコードを昇順に並べた
  自然数列が対応付けられる．このとき，それぞれの自然数列を辞書式順序で昇順に並べて，対応する自然数列が
  左から何番目にあるかをその頂点のコードとする．  
  　ここで，ある頂点の子の自然数の集合を昇順に並べる方法に計数ソートを用いて，
  自然数列を並べる方法に基数ソートを用いる．各レイヤーの頂点数を l_1, l_2, ..., l_k とすると，
  i番目のレイヤーの自然数列の各要素の総和 l_{i+1} 以下となるので，
  計算時間は O(l_1 + l_2 + ... + l_k) = O(n) となる．  
  　計数ソートや基数ソートではないソートと用いると O(n log n) となるが導出が良くわからない．

  # Note
  SPOJ では tmaeharaさんの O(n log n) 時間のアルゴリズムの方が高速（logは定数） orz

  # References
  - A. V. Aho, J. E. Hopcroft, and J. D. Ullman (1974):
  The Design and Analysis of Computer Algorithms.Addison-Wesley.
  - [@tmaehara: spagetthi-soucr/algorithm/tree_isomorphism.cc](https://github.com/spaghetti-source/algorithm/blob/master/graph/tree_isomorphism.cc)

  # Verified
  - [SPOJ 7826 : TREEISO - Tree Isomorphism](https://www.spoj.com/problems/TREEISO/)
*************************************************************************************/

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

// @require algorithm/other/counting_sort.cc

// -------------8<------- start of library -------8<------------------------
struct Tree {
    const int n;
    std::vector<std::vector<int>> adj;

    explicit Tree(int _n) : n(_n), adj(_n) {}
    void add_edge(const int v1, const int v2) {
        adj[v1].push_back(v2);
        adj[v2].push_back(v1);
    }

    std::vector<int> centers() {
        std::vector<int> prev(n);
        int u = 0;
        for (int i = 0; i < 2; ++i) { // double sweap
            prev.assign(n, -1);
            std::queue<int> que;
            que.push(prev[u] = u);
            while (!que.empty()) {
                u = que.front(); que.pop();
                for (auto v : adj[u]) {
                    if (prev[v] == -1) {
                        prev[v] = u;
                        que.push(v);
                    }
                }
            }
        }

        std::vector<int> path = {u};
        while (prev[u] != u) path.push_back(u = prev[u]);

        if (path.size() % 2 == 1) return {path[path.size() / 2]};
        else return {path[path.size() / 2 - 1], path[path.size() / 2]};
    }

    std::vector<std::vector<int>> layer;
    std::vector<int> prev;
    int levelize(const int root) { // split vertices into levels
        layer = {{root}};
        prev.assign(n, -1); prev[root] = n;

        while (true) {
            std::vector<int> next;
            for (int u : layer.back()) {
                for (int v : adj[u])
                    if (prev[v] == -1) {
                        prev[v] = u;
                        next.push_back(v);
                    }
            }
            if (next.empty()) break;
            layer.emplace_back(next);
        }

        return layer.size();
    }
};

void RadixSort(std::vector<std::pair<bool, int>> &idx, const int len,
               const int ub, const std::vector<std::vector<int>> &lcS,
               const std::vector<std::vector<int>> &lcT) {
    const int n = idx.size();

    // counting sort
    std::vector<int> cnt(ub + 1);
    auto tmp = idx;
    for (int i = len - 1; 0 <= i; --i) {
        cnt.assign(ub + 1, 0);
        for (const auto &it : idx) {
            auto &code = (it.first ? lcS[it.second] : lcT[it.second]);
            if ((int)code.size() <= i) ++cnt[0];
            else ++cnt[code[i]];
        }
        for (int j = 1; j <= ub; ++j) cnt[j] += cnt[j - 1];

        for (int j = n - 1; 0 <= j; --j) {
            auto &code = (idx[j].first ? lcS[idx[j].second] : lcT[idx[j].second]);

            if ((int)code.size() <= i) {
                tmp[cnt[0] - 1] = idx[j];
                --cnt[0];
            }
            else {
                tmp[cnt[code[i]] - 1] = idx[j];
                --cnt[code[i]];
            }
        }
        for (int j = 0; j < n; ++j) idx[j] = tmp[j];
    }
}

bool Isomorphic(Tree S, const int s, Tree T, const int t) {
    if (S.levelize(s) != T.levelize(t)) return false;

    std::vector<std::vector<int>> longcodeS(S.n + 1), longcodeT(T.n + 1);
    std::vector<int> codeS(S.n), codeT(T.n);

    int max_len_code = 0, max_element = 0;

    for (int h = S.layer.size() - 1; 0 <= h; --h) {
        for (int v : S.layer[h])
            CountingSort(longcodeS[v].begin(), longcodeS[v].end());
        for (int v : T.layer[h])
            CountingSort(longcodeT[v].begin(), longcodeT[v].end());

        std::vector<std::pair<bool, int>> idx;
        for (int v : S.layer[h]) idx.push_back({true, v});
        for (int v : T.layer[h]) idx.push_back({false, v});

        // Radix Sort
        RadixSort(idx, max_len_code, max_element, longcodeS, longcodeT);

        int id = 1;
        max_len_code = 0;
        for (size_t i = 0; i < idx.size(); ++i) {
            if (i != 0) {
                auto &lc1 = (idx[i - 1].first ? longcodeS[idx[i - 1].second]
                             : longcodeT[idx[i - 1].second]);
                auto &lc2 = (idx[i].first ? longcodeS[idx[i].second]
                             : longcodeT[idx[i].second]);
                if (lc1 != lc2) ++id;
            }

            if (idx[i].first) {
                codeS[idx[i].second] = id;
                longcodeS[S.prev[idx[i].second]].push_back(id);
                max_len_code = std::max(max_len_code,
                    (int)longcodeS[S.prev[idx[i].second]].size());
            }
            else {
                codeT[idx[i].second] = id;
                longcodeT[T.prev[idx[i].second]].push_back(id);
                max_len_code = std::max(max_len_code,
                                        (int)longcodeS[T.prev[idx[i].second]].size());
            }
        }
        max_element = id;
    }

    return codeS[s] == codeT[t];
}

bool Isomorphic(Tree S, Tree T) {
    // Suppose that S and T are trees (connected and edge size == n - 1).
    if (S.n != T.n) return false;
    if (S.n <= 2) return true;

    auto s = S.centers(), t = T.centers();
    if (s.size() != t.size()) return false;
    if (Isomorphic(S, s[0], T, t[0])) return true;
    return s.size() == 2 &&  Isomorphic(S, s[1], T, t[0]);
}

// -------------8<------- end of library ---------8-------------------------

int main() {
    int n_case;
    std::cin >> n_case;
    for (int i_case = 0; i_case < n_case; ++i_case) {
        int n, v[2];

        std::cin >> n;
        Tree S(n), T(n);
        for (int i = 0; i < n - 1; ++i) {
            std::cin >> v[0] >> v[1];
            S.add_edge(v[0] - 1, v[1] - 1);
        }
        for (int i = 0; i < n - 1; ++i) {
            std::cin >> v[0] >> v[1];
            T.add_edge(v[0] - 1, v[1] - 1);
        }

        if (Isomorphic(S, T)) std::cout << "YES\n";
        else std::cout << "NO\n";
    }

    return 0;
}
