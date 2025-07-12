/*
  Is it true that std::vector<bool> is slow?
  ==========================================
  * Assumed usage is only like a flag (access, assignment, negation)

  Comparing the execution time in 4 data structures
    1. boost::dynamic_bitset<>
    2. std::vector<bool>
    3. std::vector<char>
    4. std::vector<int>

  My opinion
    small size of a data and only using sequcence iteration -> vector<char>
    otherwise -> vector<bool> or dynamic_bitset (if you can use the boost)

  Result executed this code on my computer.
    - OS : Ubuntu 18.04.1 LTS (Bionic Beaver)
    - Memory: 7.7 GiB
    - Processor: Intel Core i7-5600U CPU @ 2.60GHz x 4
    - Disk: SSD 491.2GB
    - Compiler : gcc version 7.3.0, clang version 6.0.0
  ==============================================================================
                               gcc version 7.3.0        clang version 6.0.0
  ==============================================================================
  n = 1000                  sequcence     random       sequcence     random
  boost::dynamic_bitset<> :         1,         1 [ms]          0,         1 [ms]
        std::vector<bool> :         1,         1 [ms]          0,         1 [ms]
        std::vector<char> :         0,         0 [ms]          0,         0 [ms]
         std::vector<int> :         0,         0 [ms]          0,         0 [ms]

  n = 10000                 sequcence     random       sequcence     random
  boost::dynamic_bitset<> :        10,        13 [ms]          9,        17 [ms]
        std::vector<bool> :        11,        14 [ms]          9,        17 [ms]
        std::vector<char> :         4,        11 [ms]          3,         9 [ms]
         std::vector<int> :         4,        11 [ms]          2,        10 [ms]

  n = 100000                sequcence     random       sequcence     random
  boost::dynamic_bitset<> :       117,       150 [ms]        106,       211 [ms]
        std::vector<bool> :       116,       148 [ms]        110,       209 [ms]
        std::vector<char> :        48,       151 [ms]         43,       161 [ms]
         std::vector<int> :        44,       173 [ms]         29,       187 [ms]

  n = 1000000               sequcence     random       sequcence     random
  boost::dynamic_bitset<> :      1113,      1636 [ms]        972,      2051 [ms]
        std::vector<bool> :      1162,      1615 [ms]        975,      2066 [ms]
        std::vector<char> :       475,      2706 [ms]        366,      2632 [ms]
         std::vector<int> :       485,      2647 [ms]        303,      2639 [ms]

  n = 5000000               sequcence     random       sequcence     random
  boost::dynamic_bitset<> :      5563,     12924 [ms]       4823,     14937 [ms]
        std::vector<bool> :      6635,     12717 [ms]       5617,     14985 [ms]
        std::vector<char> :      2837,     16630 [ms]       2277,     16864 [ms]
         std::vector<int> :      2606,     36168 [ms]       2410,     37181 [ms]

  n = 10000000              sequcence     random       sequcence     random
  boost::dynamic_bitset<> :     11938,     27350 [ms]      10334,     30573 [ms]
        std::vector<bool> :     13275,     27192 [ms]      11431,     30518 [ms]
        std::vector<char> :      5723,     58778 [ms]       4470,     57117 [ms]
         std::vector<int> :      5404,     82234 [ms]       4745,     81819 [ms]
  ------------------------------------------------------------------------------
*/

#include <iostream>
#include <vector>
#include <chrono>
#include <functional>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <random>
#include <boost/dynamic_bitset.hpp>

template<typename T>
void Solve(const bool flag, const int n, T &&d, const std::vector<int> &idx) {
    constexpr int size_loop = 100;

    if (flag) { // sequence iteration
        for (int i = 0; i < size_loop; ++i) {
            for (int j = 0; j < n; ++j) d[j] = ((i + j) % 2 == 0);
            for (int j = 0; j < n; ++j) d[j] = !d[j];
            for (int j = 0; j < n; ++j) if (!d[j]) d[j] = true;
            for (int j = n - 1; 0 <= j; --j) d[j] = ((i + j) % 5 == 0);
            for (int j = 0; j < n; ++j) if (d[j]) d[j] = false;
            for (int j = n - 1; 0 <= j; --j) d[j] = !d[j];
        }
    }
    else { // random access iteration
        for (int i = 0; i < size_loop; ++i) {
            for (const int j : idx) d[j] = ((i + j) % 2 == 0);
            for (const int j : idx) d[j] = !d[j];
            for (const int j : idx) if (!d[j]) d[j] = true;
            for (const int j : idx) d[j] = ((i + j) % 5 == 0);
            for (const int j : idx) if (d[j]) d[j] = false;
        }
    }
}

int main() {

    std::vector<std::string> name = {
        "boost::dynamic_bitset<>",
        "      std::vector<bool>",
        "      std::vector<char>",
        "       std::vector<int>",
    };
    std::vector<std::function<void(bool, int, std::vector<int>)>> func = {
        [](const bool flag, const int n, const std::vector<int> &idx)
        { boost::dynamic_bitset<> d(n); Solve(flag, n, d, idx); },

        [](const bool flag, const int n, const std::vector<int> &idx)
        { std::vector<bool> d(n); Solve(flag, n, d, idx); },

        [](const bool flag, const int n, const std::vector<int> &idx)
        { std::vector<char> d(n); Solve(flag, n, d, idx); },

        [](const bool flag, const int n, const std::vector<int> &idx)
        { std::vector<int> d(n); Solve(flag, n, d, idx); },
    };

    // parameter
    const int size_algo = name.size(), size_loop = 20;
    const std::vector<int> size_data = {1000, 10000, 100000, 1000000,
                                        5000000, 10000000};

    namespace cn = std::chrono;

    for (const int n : size_data) {
        std::cout << "n = " << n << std::string(21 - (int)log10(n), ' ')
                  << "sequcence     random" << std::endl;

        // make random access iterator
        std::vector<int> idx(n); std::iota(idx.begin(), idx.end(), 0);

        // measure average execution time
        std::vector<cn::milliseconds> sum_time1(size_algo), sum_time2(size_algo);
        for (int i = 0; i < size_loop; ++i) {
            std::shuffle(idx.begin(), idx.end(), std::mt19937(0));

            for (int j = 0; j < size_algo; ++j) {
                // count time : sequcence access
                auto start = cn::high_resolution_clock::now();
                func[j](true, n, idx);
                auto end = cn::high_resolution_clock::now();
                sum_time1[j] += cn::duration_cast<cn::milliseconds>(end - start);

                // count time : random access
                start = cn::high_resolution_clock::now();
                func[j](false, n, idx);
                end = cn::high_resolution_clock::now();
                sum_time2[j] += cn::duration_cast<cn::milliseconds>(end - start);
            }
        }

        // Output average execution times
        for (int i = 0; i < size_algo; ++i) {
            std::cout << name[i] << " : " << std::setw(9) << std::right
                      << sum_time1[i].count() / size_loop << ", " << std::setw(9)
                      << std::right << sum_time2[i].count() / size_loop << " [ms]\n";
        }
        std::cout << std::endl;
    }

    return 0;
}
