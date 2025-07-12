/*
  Union Find Data Structure
  素集合データ構造
  =========================

  # Description
    - each element label is in [0, N)
    - unite(x, y): merge sets containing x and y.
    - find(x, y): return true if x and y are in the same set
    - size(x): return the size of the set containing x
    - root(x): parent label of x

  # Complexity
    Amortized O(a(n)) for all operations.
    a(n) is the inverse Ackermann function.
    In almost situations a(n) <= 4.

  # References
    - [AtCoder Union-Find](https://www.slideshare.net/chokudai/union-find-49066733)
    - [@tmaehara, spagetthi-source Union Find]
    (https://github.com/spaghetti-source/algorithm/blob/master/data_structure/union_find.cc)

  # Verified
    - [Atcoder Typical Contest001: B - Union Find]
      (https://beta.atcoder.jp/contests/atc001/tasks/unionfind_a)

*/

#include <iostream>
#include <vector>

// -------------8<------- start of library -------8<------------------------
struct UnionFind {
    std::vector<int> data;

    explicit UnionFind() {}
    explicit UnionFind(int size) : data(size, -1) {}

    bool unite(int x, int y) {
        x = root(x); y = root(y);
        if (x != y) {
            if (data[y] < data[x]) std::swap(x, y);
            data[x] += data[y]; data[y] = x;
        }
        return x != y;
    }
    bool find(int x, int y) { return root(x) == root(y); }
    int size(int x) { return -data[root(x)]; }
    int root(int x) { return data[x] < 0 ? x : data[x] = root(data[x]); }
};
// -------------8<------- end of library ---------8-------------------------


int main() {
    std::cout << std::boolalpha;

    UnionFind uf(5); // data[0, 5)

    uf.unite(1, 2);
    uf.unite(0, 4);
    uf.unite(3, 4);
    std::cout << uf.find(1, 2) << std::endl; // true
    std::cout << uf.find(1, 3) << std::endl; // false
    std::cout << uf.size(1) << std::endl;    // 2 ({1, 2})
    std::cout << uf.size(4) << std::endl;    // 3 ({0, 3, 4})
    std::cout << uf.root(1) << std::endl;    // 1
    std::cout << uf.root(4) << std::endl;    // 0

    return 0;
}
