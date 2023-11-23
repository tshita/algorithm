#include <png++/png.hpp>

#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cmath>

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
    auto get_area = [sub_size, W, H](int x, int y, u_int idx) -> std::tuple<u_int, u_int, u_int, u_int> {
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
                const auto [lx, ly, ux, uy] = get_area(x, y, idx);
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