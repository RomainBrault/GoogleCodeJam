#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <algorithm>

#define CHECK_STREAM(S, E) do { \
    if (!(S)) goto E; \
} while (0)

#define CHECK_END_STREAM(S, E) do { \
    (S).ignore(2); \
    if (!(S).eof()) goto E; \
} while (0)

inline auto count_digits(size_t arg)
    -> size_t
{
    return snprintf(NULL, 0, "%ld", arg) - (arg < 0);
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

    std::vector<size_t> base;

    size_t n = 0;
    CHECK_STREAM(file >> n, PARSE_ERROR);
    base.resize(n);

    for (size_t i = 0; i < n; ++i) {
        CHECK_STREAM(file >> base[i], PARSE_ERROR);
    }
    CHECK_END_STREAM(file, PARSE_ERROR);

#pragma omp parallel num_threads(std::atoi(argv[2]))
{
    std::array<bool, 10> visited;
    std::fill(visited.begin(), visited.end(), false);
#pragma omp for
    for (size_t i = 0; i < n; ++i) {
        size_t current = base[i];
        if (base[i] == 0) {
            continue;
        }
        while (!std::all_of(visited.begin(),
                            visited.end(), [](bool b){return b;})) {
            size_t nd = count_digits(current);
            for (size_t j = 0, c_current = current; j < nd; ++j) {
                visited[c_current % 10] = true;
                c_current /= 10;
            }
            current += base[i];
        }
        std::fill(visited.begin(), visited.end(), false);
        base[i] = current - base[i];
    }
}
    for (size_t i = 0; i < n; ++i) {
        std::cout << "Case #" + std::to_string(i + 1) + ": ";
        if (base[i] == 0){
            std::cout << "INSOMNIA" << std::endl; continue;
        }
        std::cout << base[i] << std::endl;
    }

    return 0;
PARSE_ERROR:
    std::cerr << "error while parsing file" << std::endl;
    return -2;
}