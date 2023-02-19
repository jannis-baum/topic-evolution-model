#ifndef YAML_HPP
#define YAML_HPP

#include <functional>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "topics.hpp"

// couldn't get the implicit template instantiation to compile if i put the
// definition in a source file so now they're here - jannis

template<typename Container>
std::string dumpIterable(
    const Container &data,
    std::function<std::string(typename Container::value_type)> dumpElement
) {
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

template <typename... Tp>
std::string dumpTuple(const std::tuple<Tp...> &data, const std::vector<std::string> &labels) {
    std::string dump = "";
    std::apply([&dump, &labels](auto const &...elements) { 
      int i = 0; 
      // variadic fold expression to (compile time) "iterate" over tuple
      ((dump += labels[i++] + ": " + std::to_string(elements) + "\n"), ...);
    }, data);
    return dump;
}

#endif
