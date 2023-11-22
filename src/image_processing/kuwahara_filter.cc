#include <png++/png.hpp>

#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cmath>

using Image = png::image<png::rgb_pixel>;

Image KuwaharaFilter(const Image &org, u_int window_size = 5) {
    const u_int sub_size = window_size / 2;
    Image img(org.get_width(), org.get_height());

    using SumPix = std::tuple<u_int, u_int, u_int>;
    const int dx[] = {-1, 0, -1, 0};
    const int dy[] = {-1, -1, 0, 0};

    for (u_int y = 0; y < org.get_height(); ++y) {
        for (u_int x = 0; x < org.get_width(); ++x) {
            std::pair<double, SumPix> select(DBL_MAX, SumPix()); // (variance, mean)

            for (u_int i = 0; i < 4; ++i) {
                u_int sum = 0, sum_sqr = 0, num = 0;
                SumPix sum_pix(0, 0, 0);

                const u_int sy = std::clamp(y + sub_size * dy[i], 0u, y);
                const u_int sx = std::clamp(x + sub_size * dx[i], 0u, x);

                for (u_int i = 0; i <= sub_size && sy + i < org.get_height(); ++i) {
                    for (u_int j = 0; j <= sub_size && sx + j < org.get_width(); ++j) {
                        const png::rgb_pixel &p = org[sy + i][sx + j];
                        ++num;

                        // HSV色空間の明度を取得（R, G, B の最大値に等しい）
                        const u_int value = std::max({p.red, p.green, p.blue});

                        sum += value;
                        sum_sqr += value * value;
                        std::get<0>(sum_pix) += p.red;
                        std::get<1>(sum_pix) += p.green;
                        std::get<2>(sum_pix) += p.blue;
                    }
                }

                const double var = sum_sqr / num - (sum / num) * (sum / num);
                if (num > 1 && select.first > var) {
                    select.first = var;
                    std::get<0>(select.second) = std::round(std::get<0>(sum_pix) / num);
                    std::get<1>(select.second) = std::round(std::get<1>(sum_pix) / num);
                    std::get<2>(select.second) = std::round(std::get<2>(sum_pix) / num);
                }
            }

            // Update
            img[y][x].red = std::get<0>(select.second);
            img[y][x].green = std::get<1>(select.second);
            img[y][x].blue = std::get<2>(select.second);
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