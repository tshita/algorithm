/*
  Gaussian Elimination With Pivoting
  枢軸選択付きガウスの消去法
  ==================================

  # Problem
    Input: n x n 正方行列 A, n 次元列ベクトル b
    Output: Ax = b を満たす n 次元列ベクトル x

  # Complexity
    - Time: O(n^3)　（前進消去でほぼ n^3 / 3 回，後進代入でほぼ n^2 / 2 回）

  # Usage
    - Vector GaussianElimination(A, b): A と b を入力として x を返す
    　　　　　　　　　　　　　　　　　　　　　　　（A が正則行列でないとき空ベクトルを返す）

  # Description
    ガウスの消去法（gaussian elimination）または掃き出し法（row reduction）とは
    連立一次方程式を解く直接法のアルゴリズム．前進消去で上三角行列にして，後進代入で x の値を求める．
    枢軸（pivot）は絶対値が最大となるものを選択している．ただし，EPS よりも小さいときは 0 とみなしている．
    A が正則であることと A のランクが n であることは必要十分なので，枢軸として 0 のものがあるときは
    解なしとして空ベクトルを返す．

  # Note
    - 引数の A と b は値渡しで内部で破壊される
    - 誤差が大きいので注意（求めた Ax の値と b を比較すると 100 * EPS よりも大きい場合がある）

  # References
    - [数値解析（松尾）　配付資料]
      (http://www.sr3.t.u-tokyo.ac.jp/~matsuo/lecture/12suchi/shiryo2.pdf)

  # Verified

*/

#include <iostream>
#include <vector>
#include <cassert>

// -------------8<------- start of library -------8<------------------------
using Number = double;
using Vector = std::vector<Number>;
using Matrix = std::vector<Vector>;
constexpr Number EPS = 1e-12;

Vector GaussianElimination(Matrix A, Vector b) {
    const size_t N = A.size();
    assert(0 < N && N == A[0].size() && N == b.size());

    // 前進消去
    for (size_t k = 0; k + 1 < N; ++k) {
        size_t pivot = k;
        for (size_t i = k + 1; i < N; ++i)
            if (std::abs(A[pivot][k]) < std::abs(A[i][k])) pivot = i;
        if (std::abs(A[pivot][k]) <= EPS) return Vector{};
        std::swap(A[k], A[pivot]); std::swap(b[k], b[pivot]);

        for (size_t i = k + 1; i < N; ++i) {
            A[i][k] = A[i][k] / A[k][k];
            for (size_t j = k + 1; j < N; ++j)
                A[i][j] -= A[i][k] * A[k][j];
            b[i] -= A[i][k] * b[k];
        }
    }

    // 後退代入
    for (int i = N - 1; 0 <= i; --i) {
        Number sum = 0;
        for (size_t j = i + 1; j < N; ++j) sum += A[i][j] * b[j];
        b[i] = (b[i] - sum) / A[i][i];
    }

    return b;
}
// -------------8<------- end of library ---------8-------------------------

int main() {
    std::cin.tie(0); std::ios::sync_with_stdio(false);

    Matrix A = {{1, 0}, {0, 1}};
    Vector b = {1, 2};

    Vector x = GaussianElimination(A, b);
    for (const auto &e : x) std::cout << e << std::endl;

    return 0;
}
