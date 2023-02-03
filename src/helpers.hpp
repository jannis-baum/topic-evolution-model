#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <vector>

#include "types.hpp"

// compute commonly used threshold of
// `median(values) + standardDeviation(values) * param`
dec_t mstdThreshold(std::vector<dec_t> values, dec_t param);

#endif
