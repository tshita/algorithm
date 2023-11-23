[🏠 Home](../../README.md)

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


# Source Code
PNG 画像の読み込みと出力は [png++](https://www.nongnu.org/pngpp/doc/0.2.9/index.html) ライブラリを使用している。基本的な使い方は [png++ ＠忘れても大丈夫](https://kyopro.hateblo.jp/entry/2023/02/01/145344) を参照。

## 愚直な実装
上の説明の通りに実装した。ウィンドウサイズが大きくなると遅くなる。並列化を試してみたが下の累積和の計算より速くならなかったので、下のソースコードはシングルスレッドでの実装を載せいている。

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

Image KuwaharaFilter(const Image &org, u_int window_size = 5) {
    const u_int W = org.get_width(), H = org.get_height();
    Image img(W, H);

    using ld = long double;
    const int sub_size = window_size / 2;
    auto get_range = [sub_size, W, H](int x, int y, u_int idx) -> std::tuple<u_int, u_int, u_int, u_int> {
        u_int lx = x, ly = y, ux = x, uy = y;
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

    for (u_int y = 0; y < H; ++y) {
        for (u_int x = 0; x < W; ++x) {
            ld min_variance = LDBL_MAX;

            for (u_int idx = 0; idx < 4; ++idx) {
                const auto [lx, ly, ux, uy] = get_range(x, y, idx);
                const u_int num = (ux - lx + 1) * (uy - ly + 1);
                if (num == 1) continue;

                ld sum = 0.0, sum_p2 = 0.0;
                std::tuple<u_int, u_int, u_int> sum_pix(0, 0, 0);
                for (u_int yy = ly; yy <= uy; ++yy) {
                    for (u_int xx = lx; xx <= ux; ++xx) {
                        const png::rgb_pixel &p = org[yy][xx];

                        // HSV色空間の明度を取得（R, G, B の最大値に等しい）
                        const u_int value = std::max({p.red, p.green, p.blue});
                        sum += value;
                        sum_p2 += value * value;
                        std::get<0>(sum_pix) += p.red;
                        std::get<1>(sum_pix) += p.green;
                        std::get<2>(sum_pix) += p.blue;
                    }
                }

                // 標準偏差が最小の領域と分散が最小の領域は等しいので分散で計算
                const ld var = sum_p2 / num - std::pow(sum / num, 2.0);

                // Update
                if (var < min_variance) {
                    min_variance = var;
                    img[y][x].red = std::round(std::get<0>(sum_pix) / (ld)num);
                    img[y][x].green = std::round(std::get<1>(sum_pix) / (ld)num);
                    img[y][x].blue = std::round(std::get<2>(sum_pix) / (ld)num);
                }
            }
        }
    }
    return img;
}
// ---------------------8<------- end of library   -------8<---------------------

int main(int argc, char **argv) {
    std::string file_path;
    u_int window_size = 5;
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

u_int Sum(const std::vector<std::vector<u_int>> &v, const u_int sx, const u_int sy, const u_int rx, const u_int ry) {
    return v[ry + 1][rx + 1] - v[ry + 1][sx] - v[sy][rx + 1] + v[sy][sx];
}

Image KuwaharaFilter(const Image &org, u_int window_size = 5) {
    const u_int W = org.get_width(), H = org.get_height();

    Image img(W, H);
    std::vector<std::vector<u_int>> sum(H + 1, std::vector<u_int>(W + 1, 0));
    std::vector<std::vector<u_int>> sum_p2(H + 1, std::vector<u_int>(W + 1, 0));
    std::vector<std::vector<u_int>> sum_r(H + 1, std::vector<u_int>(W + 1, 0));
    std::vector<std::vector<u_int>> sum_g(H + 1, std::vector<u_int>(W + 1, 0));
    std::vector<std::vector<u_int>> sum_b(H + 1, std::vector<u_int>(W + 1, 0));

    for (u_int y = 0; y < H; ++y) {
        for (u_int x = 0; x < W; ++x) {
            const png::rgb_pixel &p = org[y][x];
            const u_int value = std::max({p.red, p.green, p.blue});
            sum[y + 1][x + 1] = sum[y + 1][x] + sum[y][x + 1] - sum[y][x] + value;
            sum_p2[y + 1][x + 1] = sum_p2[y + 1][x] + sum_p2[y][x + 1] - sum_p2[y][x] + value * value;
            sum_r[y + 1][x + 1] = sum_r[y + 1][x] + sum_r[y][x + 1] - sum_r[y][x] + p.red;
            sum_g[y + 1][x + 1] = sum_g[y + 1][x] + sum_g[y][x + 1] - sum_g[y][x] + p.green;
            sum_b[y + 1][x + 1] = sum_b[y + 1][x] + sum_b[y][x + 1] - sum_b[y][x] + p.blue;
        }
    }

    const int sub_size = window_size / 2;
    auto get_range = [sub_size, W, H](int x, int y, u_int idx) -> std::tuple<u_int, u_int, u_int, u_int> {
        u_int lx = x, ly = y, ux = x, uy = y;
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
    for (u_int y = 0; y < H; ++y) {
        for (u_int x = 0; x < W; ++x) {
            ld min_var = LDBL_MAX;

            for (u_int idx = 0; idx < 4; ++idx) {
                const auto [lx, ly, ux, uy] = get_range(x, y, idx);
                const u_int num = (ux - lx + 1) * (uy - ly + 1);
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
    u_int window_size = 5;
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
