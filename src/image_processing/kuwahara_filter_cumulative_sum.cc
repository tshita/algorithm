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