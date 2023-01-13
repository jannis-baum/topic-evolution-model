#include <cstdlib>
#include <iostream>
#include <vector>

#include "Document.hpp"

int main(int argc, char* argv[]) {
    std::vector<int> numbers = {};

    for (int i = 1; i < argc; i++) {
        numbers.push_back(atoi(argv[i]));
    }

    Document doc(numbers);

    std::cout << doc << std::endl;

    std::cout << "nutrition for 2: " << doc.nutrition(2, 1) << std::endl;
}
