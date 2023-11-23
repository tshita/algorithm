#include <png++/png.hpp>

#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cmath>

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