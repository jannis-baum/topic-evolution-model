#include <cmath>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <string>
#include <sstream>

#include <cpr/cpr.h>

#include "helpers.hpp"

dec_t mstdThreshold(std::vector<dec_t> values, dec_t param) {
    size_t count = values.size();

    // compute mean
    dec_t mean = 0;
    for (const auto value : values) mean += value;
    mean /= count;

    // compute standard deviation
    dec_t std_deviation = 0;
    for (const auto value : values) {
        std_deviation += std::pow(value - mean, 2);
    }
    std_deviation = std::sqrt(std_deviation / count);

    // compute median
    size_t nth = count / 2;
    // sort so that all values up to nth are in correct order
    std::nth_element(values.begin(), values.begin() + nth, values.end());
    dec_t median = values[nth];

    return mean + std_deviation * param;
}

dec_t **wordDistances(const std::vector<std::string> words) {
    std::stringstream body;
    if (!words.empty()) {
        std::copy(words.begin(), std::prev(words.end()), std::ostream_iterator<std::string>(body, "\n"));
        body << words.back();
    }

    cpr::Response r = cpr::Post(
        cpr::Url{"localhost:8000/similarity"},
        cpr::Body{body.str()},
        cpr::Header{{"Content-Type", "text/plain"}}
    );
    std::cout << r.text << std::endl;
    return NULL;
}
