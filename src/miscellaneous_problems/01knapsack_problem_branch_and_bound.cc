/*
  0-1 Knapsack Problem (branching and bound)
  0-1ナップサック問題（分枝限定法）
  ==========================================

  # Problem
    Input: ナップサックの重量W, n個のアイテム I = {(w_1, v_1), ..., (w_n, v_n)}
           w_i := i番目のアイテムの重さ，v_i := i番目のアイテムの価値
    Output: \sum_{i \in X} w_i <= W で \sum_{i \in X} v_i が最大となる X \subseteq [n]

  # Complexity
    Time : O(n 2^n) (限定操作が上手く働くインスタンスではより高速)

  # Description
    この問題の線形緩和問題は，v_i / w_i を降順にソートして先頭から貪欲に選ぶと最適解となる．
    任意の順番でアイテムを選ぶ・選ばないでという操作を分枝操作とする．現在の最良値を v* として，
    現在の部分問題の線形緩和問題の最適値を v' としたとき，v' < v* ならその部分問題を解かない
    とする限定操作を行う．

  # References
    - [岡本吉央，最適化手法 第3回 整数計画法(3) : 緩和問題とその威力]
      (http://dopal.cs.uec.ac.jp/okamotoy/lect/2013/opt/lect03.pdf)
    - [Taknori Maehara, Github algorithm/other/knapsack_expcore.cc]
      (https://github.com/spaghetti-source/algorithm/blob/master/other/knapsack_expcore.cc)

  # Verified
    - [AOJ Combinatorial - 0-1 Knapsack Problem]
      (http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=DPL_1_B)
      Case#34でTLE（他はAC）
    - [Instances of 0/1 Knapsack Problem]
      (http://artemisa.unicauca.edu.co/~johnyortega/instances_01_KP/)
      large_scale/knapPI_3_*_1 (2000 <= *) は10分以上かかったので打ち切り
*/

#include <iostream>
#include <vector>
#include <algorithm>

// -------------8<------- start of library -------8<------------------------
template<class T>
class Knapsack {
public:
    const int size;
    std::vector<char> opt_item;
    T capacity, opt_v;

    Knapsack(const int _n, const T _c) :
        size(_n), opt_item(_n, false), capacity(_c), n(0), item(_n), used(_n, false) {}

    void add_item(const T v, const T w) {
        item[n].v = v;
        item[n++].w = w;
    }

    T MaximumValue() {
        std::sort(item.begin(), item.end(), [](const Item &d1, const Item &d2)
                  { return d1.v * d2.w > d2.v * d1.w; });

        opt_v = 0;
        for (int i = 0, w = 0; i < size; ++i) {
            if (w + item[i].w <= capacity) {
                w += item[i].w;
                opt_v += item[i].v;
                opt_item[i] = true;
            }
        }
        Rec(0, 0, 0);

        return opt_v;
    }

private:
    struct Item { T v , w; };
    int n;
    std::vector<Item> item;
    std::vector<char> used;

    void Rec(int idx, T value, T weight) {
        if (idx == size || capacity <= weight) {
            if (weight <= capacity && opt_v < value) {
                opt_v = value;
                opt_item = used;
            }
            return ;
        }
        if (opt_v < value) {
            opt_v = value;
            opt_item = used;
        }

        // 線形緩和したときの目的関数値の計算
        double tmp_v = value, tmp_w = weight;
        int tmp_i;
        for (tmp_i = idx; tmp_w < capacity && tmp_i < size; ++tmp_i) {
            if (tmp_w + item[tmp_i].w <= capacity) {
                tmp_v += item[tmp_i].v;
                tmp_w += item[tmp_i].w;
            }
            else if (tmp_w + item[tmp_i].w == capacity) {
                // 緩和解が元の実行可能解
                tmp_v += item[tmp_i].v;
                if (opt_v < tmp_v) {
                    opt_v = tmp_v;
                    opt_item = used;
                    for (int i = idx; i <= tmp_i; ++i)
                        opt_item[i] = true;
                }
                return ;
            }
            else {
                tmp_v += item[tmp_i].v * (capacity - tmp_w) / item[tmp_i].w;
                tmp_w = capacity;
            }
        }
        if (tmp_v <= opt_v) return;

        if (weight + item[idx].w <= capacity) {
            used[idx] = true;
            Rec(idx + 1, value + item[idx].v, weight + item[idx].w);
            used[idx] = false;
        }

        Rec(idx + 1, value, weight);
    }
};
// -------------8<------- end of library ---------8-------------------------

int main() {
    int n, cap;
    std::cin >> n >> cap;

    Knapsack<int> kp(n, cap);
    for (int i = 0, v, w; i < n; ++i) {
        std::cin >> v >> w;
        kp.add_item(v, w);
    }
    std::cout << kp.MaximumValue() << std::endl;

    return 0;
}
