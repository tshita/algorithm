[🏠 Home](../index.md#7-image-processing)

# Kuwahara filter（桑原フィルター）
- **Input**  
  RGB 形式（色深度 8 bit）の PNG 画像とウィンドウサイズ $2 a + 1$


## Description
Kuwahara filter は非線形フィルターである。  
位置 $(x, y)$ の画素は、 $(x, y)$ を中心とする一辺が $2 a + 1$ のウィンドウ内の 4 つの領域 $\lbrace Q_1, Q_2, Q_3, Q_4 \rbrace$ に対して、その中で分散が最小となる領域の平均とする。

- $Q_1 = [x, x + a] \times [y, y + a]$
- $Q_2 = [x - a, x] \times [y, y + a]$
- $Q_3 = [x - a, x] \times [y - a, y]$
- $Q_4 = [x, x + a] \times [y - a, y]$

画像がグレースケールの場合の分散と平均の計算は自明だが、RGB 画像の場合には注意が必要である。まず、RGB 色空間から HSV 色空間に変換して、明度に対して分散が最小となる領域を求め、その領域の RGB 色空間での各チャンネルに対して平均を計算して変換後の画素とする。また、明度は RGB の各チャンネルの最大値に等しい。

### Note
- Wikipedia [1] では標準偏差が最小となる領域を選択するとあるが、これは分散が最小となる領域と等しく、分散の方が平方根を計算しないで済むので分散で計算した。また、[2] でも分散で計算をしている。
- 分散は $\frac{1}{n} \sum_{i = 1}^{n} (x_i - \mu)^2$ ではなく、 $\frac{1}{n} \sum_{i = 1}^{n} x^2 - \left( \frac{1}{n} \sum_{i = 1}^{n} x_i \right)^2$ で計算した。
- 領域の形状や明度以外の分散などいろいろ種類があるらしい。


## Reference
 - [1] [Kuwahara filter @Wikipedia](https://en.wikipedia.org/wiki/Kuwahara_filter)
 - [2] [湊小太郎，桑原道義：「SPECT 用データ処理 -画像データ処理-」．](https://www.jstage.jst.go.jp/article/mit/2/2/2_91/_pdf)
 - [3] [黒田久泰：C 言語による OpenMP 入門．東京大学情報基盤センター プログラミング講習会資料．](https://www.cc.u-tokyo.ac.jp/events/lectures/03/kosyu-openmp_c.pdf)


# Source Code
PNG 画像の読み込みと出力は [png++](https://www.nongnu.org/pngpp/doc/0.2.9/index.html) ライブラリを使用している。基本的な使い方は [png++ ＠忘れても大丈夫](https://kyopro.hateblo.jp/entry/2023/02/01/145344) を参照。
```bash
$  g++ -std=c++2a -Wall -Wextra -O2 kuwahara_filter.cc `libpng-config --cflags --ldflags` -o kuwahara_filter # コンパイル
$ ./kuwahara_filter hoge.png 13 # ウィンドウサイズ 13 で hoge.png 画像をフィルタリング
```

## 愚直な実装
上の説明の通りに実装した。ウィンドウサイズが大きくなると遅くなる。~~スレッド並列化を試してみたが下の累積和の計算より速くならなかったので~~、下のソースコードはシングルスレッドでの実装を載せいている。  
（累積和よりも速くはならないがスレッド並列化で高速になったので下にソースコードを載せる）。

### Complexity
- **Time**　 $O(W \times H \times (2 a + 1)^2)$ （ $W$ と $H$ は画像の横と縦のサイズ）

<details>
<summary>src/image_processing/kuwahara_filter.cc を表示</summary>

```cpp
#include <png++/png.hpp>

#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cmath>

// ---------------------8<------- start of library -------8<--------------------
using Image = png::image<png::rgb_pixel>;

Image KuwaharaFilter(const Image &org, uint32_t window_size = 5) {
    const uint32_t W = org.get_width(), H = org.get_height();
    Image img(W, H);

    using ld = long double;
    const int sub_size = window_size / 2;
    auto get_range = [sub_size, W, H](int x, int y, uint32_t idx) -> std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> {
        uint32_t lx = x, ly = y, ux = x, uy = y;
        if (idx == 0) { // left up
            lx = std::clamp(x - sub_size, 0, (int)W - 1);
            ly = std::clamp(y - sub_size, 0, (int)H - 1);
        }
        else if (idx == 1) { // right up
            ly = std::clamp(y - sub_size, 0, (int)H - 1);
            ux = std::clamp(x + sub_size, 0, (int)W - 1);
        }
        else if (idx == 2) { // left down
            lx = std::clamp(x - sub_size, 0, (int)W - 1);
            uy = std::clamp(y + sub_size, 0, (int)H - 1);
        }
        else if (idx == 3) { // right down
            ux = std::clamp(x + sub_size, 0, (int)W - 1);
            uy = std::clamp(y + sub_size, 0, (int)H - 1);
        }
        return {lx, ly, ux, uy};
    };

    for (uint32_t y = 0; y < H; ++y) {
        for (uint32_t x = 0; x < W; ++x) {
            ld min_variance = LDBL_MAX;

            for (uint32_t idx = 0; idx < 4; ++idx) {
                const auto [lx, ly, ux, uy] = get_range(x, y, idx);
                const uint32_t num = (ux - lx + 1) * (uy - ly + 1);
                if (num == 1) continue;

                uint32_t sum = 0, sum_red = 0, sum_green = 0, sum_blue = 0;;
                uint64_t sum_p2 = 0;
                for (uint32_t yy = ly; yy <= uy; ++yy) {
                    for (uint32_t xx = lx; xx <= ux; ++xx) {
                        const png::rgb_pixel &p = org[yy][xx];

                        // HSV色空間の明度を取得（R, G, B の最大値に等しい）
                        const uint32_t value = std::max({p.red, p.green, p.blue});
                        sum += value;
                        sum_p2 += value * value;
                        sum_red += p.red;
                        sum_green += p.green;
                        sum_blue += p.blue;
                    }
                }

                // 標準偏差が最小の領域と分散が最小の領域は等しいので分散で計算
                const ld var = (ld)sum_p2 / num - std::pow((ld)sum / num, 2.0);

                // Update
                if (var < min_variance) {
                    min_variance = var;
                    img[y][x].red = std::round(sum_red / (ld)num);
                    img[y][x].green = std::round(sum_green / (ld)num);
                    img[y][x].blue = std::round(sum_blue / (ld)num);
                }
            }
        }
    }
    return img;
}
// ---------------------8<------- end of library   -------8<---------------------

int main(int argc, char **argv) {
    std::string file_path;
    uint32_t window_size = 5;
    for (int i = 0; i < argc; ++i) {
        std::string para(argv[i]);
        // Check only unsigned int window size and PNG image for simplicity.
        if (std::all_of(para.cbegin(), para.cend(), isdigit)) {
            window_size = std::stoi(para);
        }
        else if (para.ends_with(".png")) {
            file_path = para;
        }
    }

    try {
        Image img(file_path);
        auto result_img = KuwaharaFilter(img, window_size);
        result_img.write(file_path.substr(0, file_path.size() - 4) + "_kuwahara.png");
    } catch (png::error &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
```
</details>


## 愚直な実装 + OpenMP
上の愚直な実装に OpenMP を用いて画像の行分割でスレッド並列化した。コンパイルオプションにに `-fopenmp` を加える必要がある。  
下のソースコードは行のみで分割しているが、行と列で分割するためには `collapse` 指示節を加え引数に 2 を指定する必要がある。引数の 2 はループの深さに対応している。   
```cpp
#pragma omp parallel for collapse(2) num_threads(omp_get_max_threads())
```
各スレッドは画像の異なる位置のピクセルにしか書き込まず、`png::rgb_pixel` は `std::vector` で管理されているのでソースコードを変更せずに OpenMP の指示文だけで並列化できた（[stackoverflow: Is a std::vector threadsafe if the threads read and write in disjoint index ranges?](https://stackoverflow.com/questions/39163532/is-a-stdvector-threadsafe-if-the-threads-read-and-write-in-disjoint-index-rang)）。

<details>
<summary>src/image_processing/kuwahara_filter_omp.cc を表示</summary>

```cpp
#include <png++/png.hpp>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cmath>

// ---------------------8<------- start of library -------8<--------------------
using Image = png::image<png::rgb_pixel>;

Image KuwaharaFilter(const Image &org, uint32_t window_size = 5) {
    const uint32_t W = org.get_width(), H = org.get_height();
    Image img(W, H);

    using ld = long double;
    const int sub_size = window_size / 2;
    auto get_range = [sub_size, W, H](int x, int y, uint32_t idx) -> std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> {
        uint32_t lx = x, ly = y, ux = x, uy = y;
        if (idx == 0) { // left up
            lx = std::clamp(x - sub_size, 0, (int)W - 1);
            ly = std::clamp(y - sub_size, 0, (int)H - 1);
        }
        else if (idx == 1) { // right up
            ly = std::clamp(y - sub_size, 0, (int)H - 1);
            ux = std::clamp(x + sub_size, 0, (int)W - 1);
        }
        else if (idx == 2) { // left down
            lx = std::clamp(x - sub_size, 0, (int)W - 1);
            uy = std::clamp(y + sub_size, 0, (int)H - 1);
        }
        else if (idx == 3) { // right down
            ux = std::clamp(x + sub_size, 0, (int)W - 1);
            uy = std::clamp(y + sub_size, 0, (int)H - 1);
        }
        return {lx, ly, ux, uy};
    };

#pragma omp parallel for num_threads(omp_get_max_threads())
    for (uint32_t y = 0; y < H; ++y) {
        for (uint32_t x = 0; x < W; ++x) {
            ld min_variance = LDBL_MAX;

            for (uint32_t idx = 0; idx < 4; ++idx) {
                const auto [lx, ly, ux, uy] = get_range(x, y, idx);
                const uint32_t num = (ux - lx + 1) * (uy - ly + 1);
                if (num == 1) continue;

                uint32_t sum = 0, sum_red = 0, sum_green = 0, sum_blue = 0;;
                uint64_t sum_p2 = 0;
                for (uint32_t yy = ly; yy <= uy; ++yy) {
                    for (uint32_t xx = lx; xx <= ux; ++xx) {
                        const png::rgb_pixel &p = org[yy][xx];

                        // HSV色空間の明度を取得（R, G, B の最大値に等しい）
                        const uint32_t value = std::max({p.red, p.green, p.blue});
                        sum += value;
                        sum_p2 += value * value;
                        sum_red += p.red;
                        sum_green += p.green;
                        sum_blue += p.blue;
                    }
                }

                // 標準偏差が最小の領域と分散が最小の領域は等しいので分散で計算
                const ld var = (ld)sum_p2 / num - std::pow((ld)sum / num, 2.0);

                // Update
                if (var < min_variance) {
                    min_variance = var;
                    img[y][x].red = std::round(sum_red / (ld)num);
                    img[y][x].green = std::round(sum_green / (ld)num);
                    img[y][x].blue = std::round(sum_blue / (ld)num);
                }
            }
        }
    }
    return img;
}
// ---------------------8<------- end of library   -------8<---------------------

int main(int argc, char **argv) {
    std::string file_path;
    uint32_t window_size = 5;
    for (int i = 0; i < argc; ++i) {
        std::string para(argv[i]);
        // Check only unsigned int window size and PNG image for simplicity.
        if (std::all_of(para.cbegin(), para.cend(), isdigit)) {
            window_size = std::stoi(para);
        }
        else if (para.ends_with(".png")) {
            file_path = para;
        }
    }

    try {
        Image img(file_path);
        auto result_img = KuwaharaFilter(img, window_size);
        result_img.write(file_path.substr(0, file_path.size() - 4) + "_kuwahara.png");
    } catch (png::error &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
```

</details>


## 累積和を用いた実装
ウィンドウ内の 4 つの領域に対しての分散と平均の計算を累積和で求めた。

### Complexity
- **Time**　 $O(W \times H)$ （ $W$ と $H$ は画像の横と縦のサイズ）

<details>
<summary>src/image_processing/kuwahara_filter_cumulative_sum.cc を表示</summary>

```cpp
#include <png++/png.hpp>

#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cmath>

// ---------------------8<------- start of library -------8<--------------------
using Image = png::image<png::rgb_pixel>;

template<typename T>
T Sum(const std::vector<std::vector<T>> &v, const uint32_t sx, const uint32_t sy, const uint32_t rx, const uint32_t ry) {
    return v[ry + 1][rx + 1] - v[ry + 1][sx] - v[sy][rx + 1] + v[sy][sx];
}

Image KuwaharaFilter(const Image &org, uint32_t window_size = 5) {
    const uint32_t W = org.get_width(), H = org.get_height();

    Image img(W, H);
    std::vector<std::vector<uint32_t>> sum(H + 1, std::vector<uint32_t>(W + 1, 0));
    std::vector<std::vector<uint64_t>> sum_p2(H + 1, std::vector<uint64_t>(W + 1, 0));
    std::vector<std::vector<uint32_t>> sum_r(H + 1, std::vector<uint32_t>(W + 1, 0));
    std::vector<std::vector<uint32_t>> sum_g(H + 1, std::vector<uint32_t>(W + 1, 0));
    std::vector<std::vector<uint32_t>> sum_b(H + 1, std::vector<uint32_t>(W + 1, 0));

    for (uint32_t y = 0; y < H; ++y) {
        for (uint32_t x = 0; x < W; ++x) {
            const png::rgb_pixel &p = org[y][x];
            const uint32_t value = std::max({p.red, p.green, p.blue});
            sum[y + 1][x + 1] = value + sum[y + 1][x] + sum[y][x + 1] - sum[y][x];
            sum_p2[y + 1][x + 1] = value * value + sum_p2[y + 1][x] + sum_p2[y][x + 1] - sum_p2[y][x];
            sum_r[y + 1][x + 1] = p.red + sum_r[y + 1][x] + sum_r[y][x + 1] - sum_r[y][x];
            sum_g[y + 1][x + 1] = p.green + sum_g[y + 1][x] + sum_g[y][x + 1] - sum_g[y][x];
            sum_b[y + 1][x + 1] = p.blue + sum_b[y + 1][x] + sum_b[y][x + 1] - sum_b[y][x];
        }
    }

    const int sub_size = window_size / 2;
    auto get_range = [sub_size, W, H](int x, int y, uint32_t idx) -> std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> {
        uint32_t lx = x, ly = y, ux = x, uy = y;
        if (idx == 0) { // left up
            lx = std::clamp(x - sub_size, 0, (int)W - 1);
            ly = std::clamp(y - sub_size, 0, (int)H - 1);
        }
        else if (idx == 1) { // right up
            ly = std::clamp(y - sub_size, 0, (int)H - 1);
            ux = std::clamp(x + sub_size, 0, (int)W - 1);
        }
        else if (idx == 2) { // left down
            lx = std::clamp(x - sub_size, 0, (int)W - 1);
            uy = std::clamp(y + sub_size, 0, (int)H - 1);
        }
        else if (idx == 3) { // right down
            ux = std::clamp(x + sub_size, 0, (int)W - 1);
            uy = std::clamp(y + sub_size, 0, (int)H - 1);
        }
        return {lx, ly, ux, uy};
    };

    using ld = long double;
    for (uint32_t y = 0; y < H; ++y) {
        for (uint32_t x = 0; x < W; ++x) {
            ld min_var = LDBL_MAX;

            for (uint32_t idx = 0; idx < 4; ++idx) {
                const auto [lx, ly, ux, uy] = get_range(x, y, idx);
                const uint32_t num = (ux - lx + 1) * (uy - ly + 1);
                if (num == 1) continue;

                const ld sum_sub = Sum(sum, lx, ly, ux, uy);
                const ld sum_sqr_sub = Sum(sum_p2, lx, ly, ux, uy);

                // 標準偏差が最小の領域と分散が最小の領域は等しいので分散で計算
                const ld var = sum_sqr_sub / num - std::pow(sum_sub / num, 2.0);

                if (var < min_var) {
                    min_var = var;
                    img[y][x].red = std::round(Sum(sum_r, lx, ly, ux, uy) / (ld)num);
                    img[y][x].green = std::round(Sum(sum_g, lx, ly, ux, uy) / (ld)num);
                    img[y][x].blue = std::round(Sum(sum_b, lx, ly, ux, uy) / (ld)num);
                }
            }
        }
    }
    return img;
}
// ---------------------8<------- end of library   -------8<---------------------

int main(int argc, char **argv) {
    std::string file_path;
    uint32_t window_size = 5;
    for (int i = 0; i < argc; ++i) {
        std::string para(argv[i]);
        // Check only unsigned int window size and PNG image for simplicity.
        if (std::all_of(para.cbegin(), para.cend(), isdigit)) {
            window_size = std::stoi(para);
        }
        else if (para.ends_with(".png")) {
            file_path = para;
        }
    }

    try {
        Image img(file_path);
        auto result_img = KuwaharaFilter(img, window_size);
        result_img.write(file_path.substr(0, file_path.size() - 4) + "_kuwahara.png");
    } catch (png::error &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
```

</details>
