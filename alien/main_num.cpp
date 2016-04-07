#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <ios>
#include <exception>
#include <algorithm>

#define CHECK_STREAM(S) do { \
    if (!(S)) throw std::ios_base::failure("error while parsing file"); \
} while (0)

#define CHECK_END_STREAM(S) do { \
    (S).ignore(1); \
    if (!(S).eof()) throw std::ios_base::failure("error while parsing file"); \
} while (0)

class AlienData {
private:
    size_t _n_num;
    std::vector<std::string> _alien_number;
    std::vector<std::string> _source_language;
    std::vector<std::string> _target_language;

public:
    auto parse_file(const std::string & filename) {

        std::ifstream file;
        file.open(filename);

        CHECK_STREAM(file >> _n_num);

        _alien_number = std::vector<std::string>(n_num());
        _source_language = std::vector<std::string>(n_num());
        _target_language = std::vector<std::string>(n_num());

        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        for (size_t i = 0; i < n_num(); ++i) {
            CHECK_STREAM(std::getline(file, _alien_number[i], ' '));
            CHECK_STREAM(std::getline(file, _source_language[i], ' '));
            CHECK_STREAM(std::getline(file, _target_language[i], '\n'));

        }
        CHECK_END_STREAM(file);

        file.close();

        return *this;
    }

    inline auto n_num(void) const
        -> size_t
    {
        return _n_num;
    }

    inline auto source_base(size_t i)
        -> size_t
    {
        return source_language(i).size();
    }

    inline auto target_base(size_t i)
        -> size_t
    {
        return target_language(i).size();
    }

    inline auto alien_number(size_t i)
        -> std::string &
    {
        return _alien_number[i];
    }

    inline auto source_language(size_t i)
        -> std::string &
    {
        return _source_language[i];
    }

    inline auto target_language(size_t i)
        -> std::string &
    {
        return _target_language[i];
    }

    inline auto alien_number_to_source_base(size_t i)
        -> std::vector<size_t>
    {
        std::vector<size_t> res(alien_number(i).size() + 1);
        size_t j = 0;
        res[j++] = source_base(i);
        for (char c : alien_number(i)) {
            res[j++] = source_language(i).find(c);
        }
        return res;
    }

    inline auto target_base_to_alien_number(
            const std::vector<size_t> & target_base, size_t i)
        -> std::string
    {
        std::string res;
        for (size_t j = 1; j < target_base.size(); ++j) {
            res += target_language(i).at(target_base[j]);
        }
        return res;
    }
};

inline auto source_base_to_decimal(const std::vector<size_t> & source_base)
    -> size_t
{
    size_t base = source_base[0];
    size_t acc = 0;
    for (size_t i = 1; i < source_base.size(); ++i) {
        acc *= base;
        acc += source_base[i];
    }
    return acc;
}

inline auto decimal_to_target_base(size_t dec, size_t target_base)
    -> std::vector<size_t>
{
    std::vector<size_t> res;
    res.push_back(target_base);
    do {
        res.push_back(dec % target_base);
        dec /= target_base;
    } while (dec != 0);
    std::reverse(res.begin() + 1, res.end());
    return res;
}

int main(int argc, char** argv) {

    if (argc < 3) {
        std::cerr << "expected \'./programe filename\' n_threads ";
        return -1;
    }
    try {
        AlienData data;
        data.parse_file(argv[1]);
    
        std::vector<std::string> res(data.n_num());
#pragma omp parallel num_threads(std::atoi(argv[2]))
{
        std::vector<size_t> t1;
        std::vector<size_t> t2;
#pragma omp for
        for (size_t i = 0; i < data.n_num(); ++i) {
            t1 = data.alien_number_to_source_base(i);
            size_t dec = source_base_to_decimal(t1);
            t2 = decimal_to_target_base(dec, data.target_base(i));
            res[i] = data.target_base_to_alien_number(t2, i);
        }
}
        for (size_t i = 0; i < data.n_num(); ++i) {
                std::cout << "Case #" + std::to_string(i + 1) + ": "
                          << res[i] << std::endl;
        }
    } catch(std::exception& exc) {
        std::cerr << exc.what() << std::endl;
        return -2;
    }
    return 0;
}
