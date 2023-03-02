#include <cmath>
#include <algorithm>

#include "helpers.hpp"

dec_t mstdThreshold(std::vector<dec_t> values, dec_t param) {
    size_t count = values.size();

    // compute mean
    dec_t mean = 0;
    for (const auto value : values) mean += value;
    mean /= count;

    // compute standard deviation
    dec_t standardDeviation = 0;
    for (const auto value : values) {
        standardDeviation += std::pow(value - mean, 2);
    }
    standardDeviation = std::sqrt(standardDeviation / count);

    // compute median
    size_t nth = count / 2;
    // sort so that all values up to nth are in correct order
    std::nth_element(values.begin(), values.begin() + nth, values.end());
    dec_t median = values[nth];

    return mean + standardDeviation * param;
}
