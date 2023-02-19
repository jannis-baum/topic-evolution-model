#ifndef YAML_HPP
#define YAML_HPP

#include <functional>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "topics.hpp"

// couldn't get the implicit template instantiation to compile if i put the
// definition in a source file so now it's here - jannis
template<typename T>
std::string dumpVector(std::vector<T> &data, std::function<std::string(const T &)> dumpElement) {
    std::string dump = "";

    for (const auto &element : data) {
        std::string elementDump = dumpElement(element);
        std::stringstream stream(elementDump);
        std::string line;
        int i = 0;
        while (std::getline(stream, line, '\n')) {
            dump += ((i++) ? "  " : "- ") + line + "\n";
        }
    }

    return dump;
}

#endif
