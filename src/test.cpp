#include <iostream>
#include "assert.h"

#include "Corpus.hpp"

int testAll() {
    std::cout << "CORPUS PERIOD" << std::endl;
    std::cout << "Subword should point to superword" << std::endl;
    Corpus corpus = Corpus({{ { "a", "b" }, { "b" }, {"c"}, {"c"}, {"c"} }});
    assert(corpus.periods[0].wtonode.at(0).word == corpus.periods[0].wtonode.at(1).word);

    std::cout << "SUCCESS" << std::endl;
    return 0;
}
