#include "argparse.hpp"

char **getArgIt(char **begin, char **end, const std::string &arg) {
    return std::find(begin, end, arg);
}

bool hasArg(char **begin, char **end, const std::string &arg) {
    return getArgIt(begin, end, arg) != end;
}
