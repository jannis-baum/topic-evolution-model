#ifndef TEST_HPP
#define TEST_HPP

#include <cstdlib>
#include <functional>
#include <string>

#include "../types.hpp"

int testAll();
int genericTest(std::string expectation, std::function<bool()> isSuccess);
constexpr bool isEqual(dec_t a, dec_t b, dec_t epsilon = 0.001) {
    return std::abs(a - b) < epsilon;
};

int testPeriod();
int testCorpus();
int testTopics();
int testYaml();

#endif
