#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>

#define CHECK_STREAM(S, E) do { \
    if (!(S)) goto E; \
} while (0)

#define CHECK_END_STREAM(S, E) do { \
    (S).ignore(2); \
    if (!(S).eof()) goto E; \
} while (0)

inline auto flip(auto && begin, auto && end)
    -> void
{
    std::transform(begin, end, begin, [](bool b){return !b;});
    std::reverse(begin, end);
}

auto main(int argc, char** argv)
    -> int
{

    if (argc < 3) {
        std::cerr << "expected \'./programe filename\' n_threads " << std::endl;
        return -1;
    }
    std::ifstream file;
    file.open(argv[1]);

    size_t n;
    std::vector<std::vector<bool>> data;
    std::vector<size_t> iter;
    CHECK_STREAM(file >> n, PARSE_ERROR);
    data.resize(n);
    iter.resize(n);

    {
        std::string line;
        CHECK_STREAM(file.ignore(1), PARSE_ERROR);
        for (size_t i = 0; i < n; ++i) {
            CHECK_STREAM(std::getline(file, line), PARSE_ERROR);
            data[i].resize(line.size());
            for (size_t j = 0; j < line.size(); ++j) {
                data[i][j] = (line[j] == '+' ? true : false);
            }
        }
    }
    CHECK_END_STREAM(file, PARSE_ERROR);

#pragma omp parallel for num_threads(atoi(argv[2]))
    for (size_t i = 0; i < n; ++i) {
        auto dib = data[i].begin();
        auto die = data[i].end();
        iter[i] = 0;
        while (!std::all_of(dib, die, [](bool b){return b;})) {
            if (std::all_of(dib, die, [](bool b){return !b;})) {
                // flip(dib, die);
                ++iter[i]; break;
            }
            auto its = std::adjacent_find(data[i].begin(),
                                          data[i].end(),
                                          std::not_equal_to<bool>());
            flip(dib, its + 1);
            ++iter[i];
        }
    }
    for (size_t i = 0; i < n; ++i) {
        std::cout << "Case #" + std::to_string(i + 1) + ": " +
                                std::to_string(iter[i]) << std::endl;
    }

    return 0;
PARSE_ERROR:
    std::cerr << "error while parsing file" << std::endl;
    return -2;
}