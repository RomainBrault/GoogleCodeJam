#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

#define CHECK_STREAM(S) do { \
    if (!(S)) throw std::ios_base::failure("error while parsing file"); \
} while (0)

#define CHECK_END_STREAM(S) do { \
    (S).ignore(1); \
    if (!(S).eof()) throw std::ios_base::failure("error while parsing file"); \
} while (0)

class AlienData {
private:
    size_t _L;
    size_t _D;
    size_t _N;
    std::vector<std::string> _alien_words;
    std::vector<std::string> _alien_signals;

public:

public:
    auto parse_file(const std::string & filename)
    {

        std::ifstream file;
        file.open(filename);

        CHECK_STREAM(file >> _L);
        CHECK_STREAM(file >> _D);
        CHECK_STREAM(file >> _N);

        _alien_words = std::vector<std::string>(D());
        _alien_signals = std::vector<std::string>(N());

        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        for (size_t i = 0; i < D(); ++i) {
            CHECK_STREAM(std::getline(file, _alien_words[i], '\n'));
        }
        for (size_t i = 0; i < N(); ++i) {
            CHECK_STREAM(std::getline(file, _alien_signals[i], '\n'));
            bool mod = false;
            for (size_t j = 0; j < alien_signal(i).size(); ++j) {
                if (alien_signal(i)[j] == '(') {
                    mod = true; continue;
                }
                else if (alien_signal(i)[j] == ')') {
                    mod = false; continue;
                }
                else if (mod && alien_signal(i)[j+1] != ')') {
                    alien_signal(i).insert(++j, "|"); continue;
                }
            }
        }
        CHECK_END_STREAM(file);
        file.close();

        return *this;
    }

    inline auto L(void) const -> size_t
    {
        return _L;
    }

    inline auto D(void) const -> size_t
    {
        return _D;
    }

    inline auto N(void) const -> size_t
    {
        return _N;
    }

    inline auto alien_word(size_t i) -> std::string &
    {
        return _alien_words[i];
    }

    inline auto alien_signal(size_t i) -> std::string &
    {
        return _alien_signals[i];
    }

};

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "expected \'./programe filename\' n_threads ";
        return -1;
    }
try {
        AlienData data;
        data.parse_file(argv[1]);
    
        std::vector<size_t> acc(data.N());
#pragma omp parallel for num_threads(atoi(argv[2]))
        for (size_t i = 0; i < data.N(); ++i) {
            std::regex alien_signal_regex(data.alien_signal(i),
                std::regex::optimize | std::regex_constants::nosubs);
            for (size_t j = 0; j < data.D(); ++j) {
                acc[i] += std::regex_match(data.alien_word(j), alien_signal_regex);
            }
        }
        for (size_t i = 0; i < data.N(); ++i) {
            std::cout << "Case #" + std::to_string(i + 1) + ": "
                      << acc[i] << std::endl;
        }
    } catch (std::exception& exc) {
        std::cerr << exc.what() << std::endl;
        return -2;
    }
    return 0;
}
