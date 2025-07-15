> Back to [Table of Contents](./index.md)

# Data Structures
- **（2分木）ヒープ（(binary) heap）**  
    完全二分木とみなすことができる配列でヒープ条件（heap property）を満たすデータ構造。ヒープ条件は max ヒープ条件と min ヒープ条件があり、max ヒープ条件は根以外のどのノードも、そのノードの値がその親の値以下という条件である。min ヒープ条件は逆である。max ヒープは根が最大の値となるので最大値を $O(1)$ 時間で取得できる。要素の挿入と削除は $O(\log n)$ 時間で可能。ヒープの構築自体は $O(n)$ 時間で可能であり、これを用いたソートアルゴリズムはヒープソート（heap sort）と呼ばれ、全体の計算量は $O(n \log n)$ 時間である。  
    ヒープは C++ 言語の標準ライブラリに `std::priority_queue` として実装されている。実装の詳細は [1, pp. 124-139] を参照。

- **平衡二分探索木（self-balancing binary search tree）**
    - **AVL木（AVL tree, Adelson-Velskii and Landis' tree）**（c.f. [wikipedia](https://en.wikipedia.org/wiki/AVL_tree)）  
        平衡二分探索木の一種で基本操作の最悪時間計算量が $O(\log n)$ と保証されている。各ノードの左部分木と右部分木の高さの差の絶対値が 1 以下という強い条件を満たす。そのため操作のたびに回転が発生しやすく、他の平衡木と比べて定数倍のオーバーヘッドが大きいとされている。最初に考案された平衡二分探索木。
    
    - **赤黒木（red-black tree）**（c.f. [wikipedia](https://en.wikipedia.org/wiki/Red%E2%80%93black_tree)）  
        平衡二分探索木の一種で基本操作の最悪時間計算量が $O(\log n)$ と保証されている。詳しい実装は **[1, pp. 256-281]** にある。
    
    - **スプレー木（splay tree）**（c.f. [wikipedia](https://en.wikipedia.org/wiki/Splay_tree)）  
        平衡二分探索木の一種で最近アクセスした要素に素早く再アクセスできるという特徴がある。基本操作の償却時間は $O(\log n)$ だが最悪時間計算量が $O(n)$ となる。ただし、実装が楽で計算時間も実験的には悪くないらしい。
    
    - **Treap**（c.f. [wikipedia](https://en.wikipedia.org/wiki/Treap)）  
        乱択平衡二分探索木の一種。基本操作に対する平均計算量は $O(\log n)$ だが最悪時間計算量は $O(n)$ となる。各ノードにランダムに優先度を割り当て、キーに対しては二分探索木条件を、優先度に対してはヒープ条件を満たすようにする。
        - [Takuya Akiba: プログラミングコンテストでのデータ構造 2　～平衡二分探索木編～](https://www.slideshare.net/slideshow/2-12188757/12188757)


# Computational Geometry
- **線分交差問題に対する平面走査法（plane sweep algorithm）**  
    線分交差問題とは平面上にある線分の集合が与えられたときに交点をすべて答える問題。平面走査法は $O(n \log n + I \log n)$ 時間で解くアルゴリズム。ただし、$I$ は線分の間の交点数で、このように実行時間が出力サイズに依存するアルゴリズムを **出力サイズに敏感なアルゴリズム（output-sensitive algorithm）** という。詳細は **[2, pp. 21-33]** を参照。


# Numerical Analysis
- **Kahan summation algorithm**（c.f. [wikipedia](https://en.wikipedia.org/wiki/Kahan_summation_algorithm)）  
    有限精度の浮動小数点数列を数値誤差が小さくなるように総和をとるアルゴリズム


# Miscellaneous Problems
- ジョブ・スケジューリング関連のアルゴリズム：[岡本吉央：離散最適化基礎論 2025 年度「ジョブ・スケジューリングのアルゴリズム」](http://dopal.cs.uec.ac.jp/okamotoy/lect/2024/sched/)


# References
[1] T.コルメン，C.ライザーソン，R.リベスト，C.シュタイン（浅野哲夫・岩野和生・梅尾博司・山下雅史・和田幸一 訳）：『アルゴリズムイントロダクション 第3版 総合版：世界標準MIT教科書』．近代科学社，東京，2013．  
[2] M.ドバーグ，O.チョン，M.ファンクリベルド，M.オーバマーズ（浅野哲夫 訳）：『コンピュータ・ジオメトリ 計算幾何学：アルゴリズムと応用 第3版』. 近代科学社, 東京, 2010.