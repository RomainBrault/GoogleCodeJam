#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <utility>
#include <array>
#include <cinttypes>

#define MAX_N_CHECK static_cast<size_t>(100)

#define CHECK_STREAM(S, E) do { \
    if (!(S)) goto E; \
} while (0)

#define CHECK_END_STREAM(S, E) do { \
    (S).ignore(2); \
    if (!(S).eof()) goto E; \
} while (0)

#define CHECK_OPEN_STREAM(S, E) do { \
    if (!(S).is_open()) goto E; \
} while (0)

auto find_divider(__uint128_t n) {
    if (n % 2 == 0) {
        return std::pair<bool, size_t>(false, 2);
    }
    for (size_t i = 3; i < std::min(static_cast<size_t>(sqrt(n)) + 1,
                                    MAX_N_CHECK); i+=2) {
        if (n % i == 0) {
            return std::pair<bool, size_t>(false, i);
        }
    }
    return std::pair<bool, size_t>(true, n);
}

auto intepretation(std::vector<bool> & jamcoin, size_t base)
    -> __uint128_t
{
    std::reverse(jamcoin.begin(), jamcoin.end());
    __uint128_t acc = jamcoin[0];
    __uint128_t p = base;
    for (size_t j = 1; j < jamcoin.size(); ++j) {
        acc += p * jamcoin[j];
        p *= base;
    }
    std::reverse(jamcoin.begin(), jamcoin.end());
    return acc;
}

auto is_jamcoin(std::vector<bool> & jamcoin,
                std::array<size_t, 9> & validity)
    -> bool
{
    bool acc = true;
    for (size_t base = 2; base <= 10; ++base){
        auto ptest = find_divider(intepretation(jamcoin, base));
        acc = acc && !std::get<0>(ptest);
        validity[base - 2] = std::get<1>(ptest);
    }
    return acc;
}

auto inc(std::vector<bool> & jamcoin) {
    for (size_t i = jamcoin.size() - 2; i >=  0; --i) {
        bool previous = jamcoin[i];
        jamcoin[i] = !previous;
        if (!previous) break;
    }
}

auto main(int argc, char** argv)
    -> int
{

    if (argc < 3) {
        std::cerr << "expected \'./programe filename\' n_threads ";
        return -1;
    }
    size_t T = 0, N = 0, J = 0;
    std::vector<std::vector<bool>> jamcoin;
    std::vector<std::array<size_t, 9>> validity;
    std::ifstream file;
    file.open(argv[1]);
    CHECK_OPEN_STREAM(file, PARSE_ERROR);
    CHECK_STREAM(file >> T, PARSE_ERROR);
    CHECK_STREAM(file >> N, PARSE_ERROR);
    CHECK_STREAM(file >> J, PARSE_ERROR);
    CHECK_END_STREAM(file, PARSE_ERROR);

    jamcoin.resize(J);
    validity.resize(J);
    for (size_t j = 0; j < J; ++j) {
        jamcoin[j].resize(N);
    }
    jamcoin[0][0] = true; jamcoin[0][N - 1] = true;
    while (!is_jamcoin(jamcoin[0], validity[0]) &&
           !std::all_of(jamcoin[0].begin(),
                        jamcoin[0].end(),
                        [](bool b){return b;}))
    {
        inc(jamcoin[0]);
    }
    size_t j;
    for (j = 1; j < J; ++j) {
        jamcoin[j] = jamcoin[j-1];
        if (std::all_of(jamcoin[j].begin(),
                        jamcoin[j].end(),
                        [](bool b){return b;})) {
            break;
        }
        inc(jamcoin[j]);
        while (!is_jamcoin(jamcoin[j], validity[j])) {
            if (std::all_of(jamcoin[j].begin(),
                            jamcoin[j].end(),
                            [](bool b){return b;})) {
                goto OUT_LOOP;
            }
            inc(jamcoin[j]);
        }
    }

OUT_LOOP:
    J = j;

    std::cout << "Case #1:" << std::endl;
    for (size_t j = 0; j < J; ++j) {
        std::for_each(jamcoin[j].begin(), jamcoin[j].end(),
                      [](bool b){std::cout << b;});
        std::cout << ' ';
        std::for_each(validity[j].begin(), validity[j].end(),
                      [](size_t d){std::cout << d << ' ';});
        std::cout << std::endl;
    }

    return 0;
PARSE_ERROR:
    std::cerr << "error while parsing file" << std::endl;
    return -2;
}