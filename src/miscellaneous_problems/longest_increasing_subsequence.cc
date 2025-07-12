/*
  Longest increasing subsequence
  最長増加部分列
  ===============================

  # Problem
    Input: 整数列 a = (a_1, a_2, ..., a_{n})
    Output: a の最長増加部分列の添字

    Def. 増加部分列
      a の部分列 a_{i_1}, a_{i_2}, ..., a_{i_k} が a の増加部分列であるとは，
      　任意の 1 <= j < k に対して，i_j < i_{j + 1} かつ a_{i_j} < a_{i_{j + 1}}
      を満たすことである．
      また，最長の増加部分列を最長増加部分列（longest increasing subsequence; LIS）と言う．

  # Complexity
    - Time: O(n log n)
    - Space: O(n)

  # Usage
    - vector<int> idx = longestIncreasingSubsequence(vector<T> &a):
        a を入力の整数列として，a の最長増加部分列の添字として idx を返し次を満たす
          a[idx[0]] < a[idx[1]] < ... < a[idx[k]]
        狭義単調増加の場合は lower_bound を使用して，単調増加の場合は upper_bound を使用

  # Description
    dp[i] := 長さが i + 1 となる増加部分列の最終要素の最小値（存在しない場合は INF），
    として動的計画法を行う．最長増加部分列の長さは dp[i] < INF となる最大の i に等しい．
    a[i] を最終要素に追加する遷移を考えると，j = 0 または dp[j - 1] < a[i] を満たす j に対して，
    dp[j] = min(dp[j], a[i]) と更新できる．dp の値は INF を除いて単調増加しているので，
    a[i] が入る場所を二分探索することによって O(log n) 時間で更新できる．
    したがって，全体で O(n log n) 時間で最長増加部分列を求めることが可能である．

  # TODO
    - 入力を std::vector ではなくイテレータにする（decltype らへんでハマっている）

  # References
    - [@tmaehara, Spagetthi source 最長増加部分列](http://www.prefield.com/algorithm/dp/lis_fast.html)
    - あり本, pp. 64--65

  # Verified
    - [AOJ DPL_1_D](http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=DPL_1_D)

*/

#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

// -------------8<------- start of library -------8<------------------------
template<typename T>
std::vector<int> longestIncreasingSubsequence(const std::vector<T> &a) {
    if (a.empty()) return {};

    const T INF = std::numeric_limits<T>::max();
    const int size = a.size();

    std::vector<T> dp(size, INF);
    std::vector<int> idx(size);
    for (int i = 0; i < size; ++i) {
        // 狭義単調増加： a_i < a_j
        idx[i] = std::distance(dp.begin(), lower_bound(dp.begin(), dp.end(), a[i]));

        // 単調増加： a_i <= a_j
        // idx[i] = std::distance(dp.begin(), upper_bound(dp.begin(), dp.end(), a[i]));
        
        dp[idx[i]] = a[i];
    }

    int len = *max_element(idx.begin(), idx.end());
    std::vector<int> lis_idx(len + 1);
    for (int i = size - 1; 0 <= i; --i)
        if (idx[i] == len) lis_idx[len--] = i;

    return lis_idx;
}
// -------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    int n;
    std::cin >> n;

    std::vector<int> a(n);
    for (auto &a_i : a) std::cin >> a_i;

    auto lis_idx = longestIncreasingSubsequence(a);
    for (auto x : lis_idx) std::cout << a[x] << " ";
    std::cout << std::endl;
    std::cout << lis_idx.size() << std::endl;

    return 0;
}
