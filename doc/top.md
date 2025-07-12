Reference Implementations of Algorithms (Personal Collection)
==============================================================

> Language icons (e.g., <span title="C++ implementation"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></span>, <span title="Rust implementation"><img src="../img/rust.png" width="24" alt="Rust implementation" style="vertical-align: middle; background-color: white; border-radius: 20%; margin: 2px;"></span>) indicate the implementation language  and link to the corresponding source code in that language.

> **Tags**
>   
> <span title="Approximation algorithm" style="font-size: 85%; color: #0055aa; font-weight: bold; background-color: #eef; margin: 0 4px; padding: 2px 4px; border-radius: 5px; vertical-align: middle;">[Approx]</span> : Approximation algorithm  
> <span title="NP-hard problem" style="font-size: 85%; color: #aa0000; font-weight: bold; background-color: #fee; margin: 0 4px; padding: 2px 4px; border-radius: 5px; vertical-align: middle;">[NP-hard]</span> : NP-hard problem


# 1. Graph
- Bipartite Maximum Matching  
    <span title="View C++ implementation"><a href="../src/graph/bipartite_maximum_matching.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span>  Ford-Fulkerson method in $O(n (n + m))$ time
- Maximum Independent Set Problem <span title="NP-hard problem" style="font-size: 85%; color: #aa0000; font-weight: bold; background-color: #fee; margin: 0 4px; padding: 2px 4px; border-radius: 5px; vertical-align: middle;">[NP-hard]</span>    
    <span title="View C++ implementation"><a href="../src/graph/maximum_independent_set_1.4423.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Branch-and-reduce algorithm in $O^* (1.4423^n)$ time
- Hamiltonian Path Problem in Hypercube Graph  
    <span title="View C++ implementation"><a href="../src/graph/hamiltonian_path_hypercube_graph.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> $O(n 2^n)$ time algorithm
- Lexicographic breadth first search  
    <span title="View C++ implementation"><a href="../src/graph/lexicographic_bfs.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> LexBFS in $O(n + m)$ time
- Prüfer sequence  
    <span title="View C++ implementation"><a href="../src/graph/prufer_sequence.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> $O(n \log n)$ time algorithm
- Minimum Vertex Cover Problem <span title="NP-hard problem" style="font-size: 85%; color: #aa0000; font-weight: bold; background-color: #fee; margin: 0 4px; padding: 2px 4px; border-radius: 5px; vertical-align: middle;">[NP-hard]</span>  
    <span title="View C++ implementation"><a href="../src/graph/vertex_cover_by_maximal_matching_2apx.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> 2-approximation via maximal matching in $O(n + m)$ time <span title="Constant-factor approximation algorithm" style="font-size: 85%; color: #0055aa; font-weight: bold; background-color: #eef; margin: 0 4px; padding: 2px 4px; border-radius: 5px; vertical-align: middle;">[Approx]</span>


## 1.1 Recognition Problem
- Bipartite Graph Recognition  
    <span title="View C++ implementation"><a href="../src/graph/is_bipartite.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> $O(n + m)$ time algorithm
- Chordal Graph Recognition  
    <span title="View C++ implementation"><a href="../src/graph/is_chordal.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Uses LexBFS in $O(n + m)$ time
- Cactus Recognition  
    <span title="View C++ implementation"><a href="../src/graph/is_cactus.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> $O(n + m)$ time algorithm
- Eulerian Graph Recognition  
    <span title="View C++ implementation"><a href="../src/graph/hierholzer_undirected.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Hierholzer algorithm in $O(n + m)$ time
- Eulerian Digraphs Recognition  
    <span title="View C++ implementation"><a href="../src/graph/hierholzer_directed.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Hierholzer algorithm in $O(n + m)$ time

## 1.2 Graph Isomorphism Problem
- Tree Isomorphism Problem   
    <span title="View C++ implementation"><a href="../src/graph/tree_isomorphism.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> $O(n)$ time algorithm

## 1.3 Shortest Paths Problem
- Single Source Shortest Paths Problem
    - <span title="View C++ implementation"><a href="../src/graph/dijkstra.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Dijkstra algorithm: computes distances only in $O(n^2)$ time
    - <span title="View C++ implementation"><a href="../src/graph/dijkstra_heap.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Dijkstra algorithm with heap: computes distances only in $O((n + m) \log n)$ time
    - <span title="View C++ implementation"><a href="../src/graph/dijkstra_heap_sol.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Dijkstra algorithm with heap: computes distances and paths in $O((n + m) \log n)$ time
    - <span title="View C++ implementation"><a href="../src/graph/bellman_ford.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Bellman-Ford algorithm: computes distances and detectives a negative cycle in $O(n m) $ time
- Single Source Shortest Paths Problem with Edge Weights 0 or 1  
    <span title="View C++ implementation"><a href="../src/graph/01bfs.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> 0-1 BFS algorithm in $O(n + m)$ time
- All Pairs Shortest Paths Problem  
    <span title="View C++ implementation"><a href="../src/graph/floyd_warshall.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Floyd-Warshall algorithm: computes distances only in $O(n^3)$ time

## 1.4 Connected Components
- Strongly Connected Components  
    <span title="View C++ implementation"><a href="../src/graph/strongly_connected_component_kosaraju.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Kosaraju algorithm in $O(n + m)$ time
- 2-Edge-Connected Components  
    <span title="View C++ implementation"><a href="../src/graph/bridge.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Hopcroft-Tarjan algorithm: enumerates all bridges in $O(n + m)$ time
- 2-Vertex-Connected Components  
    <span title="View C++ implementation"><a href="../src/graph/articulation_point.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Enumerates all articulation points in $O(n + m)$ time

## 1.5 Tree Problems
- Lowest Common Ancestor
    - <span title="View C++ implementation"><a href="../src/graph/lca_doubling.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Uses doubling with preprocessing in $O(n \log n)$ time, query time $O (\log n)$
    - <span title="View C++ implementation"><a href="../src/graph/lca_euler_tour.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Uses Euler tour with preprocessing in $O(n)$ time, query time $O (\log n)$
- Diameter of a Tree  
    <span title="View C++ implementation"><a href="../src/graph/tree_diameter.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> $O(n)$ time algorithm

## 1.6 Maximum Flow Problem
- Maximum Flow Problem
    - <span title="View C++ implementation"><a href="../src/graph/maximum_flow_ford_fulkerson.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Ford-Fulkerson method in $O(m f)$ or $O(m^2 U)$ time
    - <span title="View C++ implementation"><a href="../doc/graph/maximum_flow_edmonds_karp.md"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Edmonds-Karp algorithm in $O(n m^2)$ time
    - <span title="View C++ implementation"><a href="../doc/graph/maximum_flow_capacity_scaling.md"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Gabow algorithm (a.k.a. capacity scaling algorithm) in $O(m^2 \log U)$ time
    - <span title="View C++ implementation"><a href="../src/graph/maximum_flow_dinic.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Dinic algorithm in $O(n m^2)$ time

- Maximum Flow Problem with Lower Bounds  
    <span title="View C++ implementation"><a href="../src/graph/maximum_flow_with_lower_bounds.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> $O(n^2 m)$ time algorithm

## 1.7 Random Graph Generators
- Undirected Graph  
    <span title="View C++ implementation"><a href="../src/graph/erdos_renyi.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Generates Erdős–Rényi random graph in $O(n^2)$ time

- Uniform Spanning Tree
    - <span title="View C++ implementation"><a href="../src/graph/uniform_spanning_tree_aldous.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Aldous algorithm using a random walk in expected $O(m C)$ time
    - <span title="View C++ implementation"><a href="../src/graph/uniform_spanning_tree_wilson.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Wilson algorithm in likely expected $O (n^3)$ time

- Random Labeled Tree
    - <span title="View C++ implementation"><a href="../src/graph/random_labelled_tree_aldous.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Aldous algorithm using a random walk in $O(n)$ time
    - <span title="View C++ implementation"><a href="../src/graph/random_labelled_tree_prufer_sequence.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Uses a Prüfer sequence in $O(n \log n)$ time
    - <span title="View C++ implementation"><a href="../src/graph/random_labelled_tree_wilson.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Wilson algorithm in likely expected $O(n^3)$ time


# 2. Data Structures
- Data Structures
    - <span title="View C++ implementation"><a href="../src/data_structure/fenwick_tree_add_element_accumulate_prefix.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Fenwick tree (add to a single element / accumulate a prefix)
    - <span title="View C++ implementation"><a href="../src/data_structure/segment_tree_update_element_accumulate_interval.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Segment tree (update a single element / accumulate an interval)
    - <span title="View C++ implementation"><a href="../src/data_structure/union_find.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Union-Find data structure
    - <span title="View C++ implementation"><a href="../src/data_structure/initializable_array_bentley.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Initializable array (Bentley method)

- Range Sum Query Problem  
    <span title="View C++ implementation"><a href="../src/data_structure/fenwick_tree_range_sum_query.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Uses Fenwick tree with query time $O(\log n)$

- Range Minimum Query and Range Maximum Query Problem  
    <span title="View C++ implementation"><a href="../src/data_structure/sparse_table_RmQ_RMQ.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Uses sparse table with preprocessing in $O(n \log n)$ time, query time $O(1)$.


# 3. Number Theory
- <span title="View C++ implementation"><a href="../src/number_theory/modular_arithmetics.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Basic modular arithmetic
- Binomial coefficients modulo a prime
    - <span title="View C++ implementation"><a href="../src/number_theory/choose_mod.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Naive method
    - <span title="View C++ implementation"><a href="../src/number_theory/choose_mod_memo.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Memoization
- <span title="View C++ implementation"><a href="../src/number_theory/gcd_lcm.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> GCD (greatest common divisor) and LCM (least common multiple) in $O(\log \min\{a, b\})$ time
- <span title="View C++ implementation"><a href="../src/number_theory/primes_const.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Compile-time prime generation in $O(n \log \log n)$ time
- <span title="View C++ implementation"><a href="../src/number_theory/extgcd.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Extended Euclidean algorithm
- <span title="View C++ implementation"><a href="../src/number_theory/prime_fact.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Prime factorization (trial division)


# 4. Numerical Analysis
- System of Linear Equations  
    <span title="View C++ implementation"><a href="../src/numerical_analysis/gaussian_elimination_with_pivoting.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Gaussian elimination with pivoting in $O(n^3)$ time


# 5. Computational Geometry
- <span title="View C++ implementation"><a href="../src/geometry/geometry_basic.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Basic 2D geometry — For practical use, consider using [CGAL](https://www.cgal.org/)
- [🔗 A collection of computational geometry problems](https://github.com/tshita/prob_comp_geo_cpp)  
  I moved my geometry library to this repository to better organize it in a problem-driven manner.
 
 
# 6. Games and Puzzles
- Make N  
    <span title="View C++ implementation"><a href="../doc/games_puzzles/make_n.md"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> 
  Example: Form $13$ using the numbers $\{2, 5, 5, 9\}$ → $(9 - 5) \times 2 + 5$


# 7. Image Processing
- Image Filtering  
    <span title="View C++ implementation"><a href="../doc/image_processing/kuwahara_filter.md"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Kuwahara filter

　
# 8. Miscellaneous Problems
- Sorting Problem  
    <span title="View C++ implementation"><a href="../src/miscellaneous_problems/counting_sort.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Counting sort in $O(n + k)$ time

- 0-1 Knapsack Problem  
    <span title="View C++ implementation"><a href="../src/miscellaneous_problems/01knapsack_problem_branch_and_bound.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Branch-and-bound algorithm in $O(n 2^n)$ time

- 2-Satisfiability Problem  
    <span title="View C++ implementation"><a href="../src/miscellaneous_problems/two_sat.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Uses [Kosaraju algorithm for strongly connected components](../src/graph/strongly_connected_component_kosaraju.cc) in $O(n + m)$ time

- Longest Increasing Subsequence Problem  
    <span title="View C++ implementation"><a href="../src/miscellaneous_problems/longest_increasing_subsequence.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Uses dynamic programming in $O(n \log n)$ time

- Pseudorandom Number Generator  
    <span title="View C++ implementation"><a href="../src/miscellaneous_problems/xor_shift128_plus.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> XorShift128+ by Blackman and Vigna

- Rating System  
    <span title="View Rust implementation"><a href="../src/miscellaneous_problems/glicko_rating_system.rs"><img src="../img/rust.png" width="24" alt="Rust implementation" style="vertical-align: middle; background-color: white; border-radius: 20%; margin: 2px;"></a></span> Glicko rating system implementation


# 9. Performance Comparison
- <span title="View C++ implementation"><a href="../src/performance_comparison/vector_bool.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Is `std::vector<bool>` really slow in C++ when used only for flag operation (access, assignment, negation)?


# 10. Templates
- <span title="View C++ implementation"><a href="../src/template/template.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Template for competitive programming
- <span title="View C++ implementation"><a href="../src/template/algorithm_template.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Template for algorithm implementation
- <span title="View C++ implementation"><a href="../src/template/marathon_template.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span> Template for marathon matches


# Useful Resources
- [Takanori Maehara's Library](https://github.com/spaghetti-source/algorithm)
- [noshi91's Library](https://github.com/noshi91/Library)
- [anta_prg's Library](https://www.dropbox.com/scl/fo/6g2xmh19lz8zsgtt5y90b/AJPGw7pFbAtWCpqHUtFVyzM?rlkey=47z5x9ega71x4hxpxuu8ky8vr&e=1)
- [Peter Braß: *Advanced Data Structures*. Cambridge University Press, 2008.](https://www-cs.ccny.cuny.edu/~peter/dstest.html)

<!-- 
- Bipartite maximum matching <span title="View C++ implementation"><a href="../graph/bipartite_maximum_matching.cc"><img src="../img/cpp.png" width="24" alt="C++ implementation" style="vertical-align: middle; margin: 2px;"></a></span>  
- Bipartite maximum matching <span title="View Rust implementation"><a href="../graph/bipartite_maximum_matching.cc"><img src="../img/rust.png" width="24" alt="Rust implementation" style="vertical-align: middle; background-color: white; border-radius: 20%; margin: 2px;"></a></span> 
-->

<!-- スタイル
- 記述の簡潔さを重視するので冠詞とピリオドを省略
- 人名由来のアルゴリズム名には所有格をつけない (e.g., Dinic algorithm) 
-->
