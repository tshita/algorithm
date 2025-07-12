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