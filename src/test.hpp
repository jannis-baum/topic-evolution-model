#ifndef TEST_HPP
#define TEST_HPP

#include <functional>
#include <string>

int testAll();
int genericTest(std::string expectation, std::function<bool()> isSuccess);

#endif
