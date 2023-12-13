#include <functional>
#include <string>
#include <stdexcept>

char **getArgIt(char **begin, char **end, const std::string &arg);
bool hasArg(char **begin, char **end, const std::string &arg);

template<typename T>
T getArgOrFail(char **begin, char **end, const std::string &arg, const std::function<T(char*)> &cast) {
    char **it = getArgIt(begin, end, arg);
    if (it == end || ++it == end) {
        throw std::invalid_argument("Missing value for " + arg);
    }
    try {
        return cast(*it);
    } catch (...) {
        throw std::invalid_argument("Incorrect data type for " + arg);
    }
}
