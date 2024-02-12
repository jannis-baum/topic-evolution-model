#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

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
    if (words.empty()) return NULL;

    // retrieve endpoint URI from environment TEM_WORD_DISTANCE_ENDPOINT
    char *endpoint = std::getenv("TEM_WORD_DISTANCE_ENDPOINT");
    if (endpoint == NULL) {
        std::cerr << "fatal: $TEM_WORD_DISTANCE_ENDPOINT not defined" << std::endl;
        std::abort();
    }

    // create request body: words split by newlines
    std::stringstream body;
    std::copy(words.begin(), std::prev(words.end()), std::ostream_iterator<std::string>(body, "\n"));
    body << words.back();

    // make request
    cpr::Response r = cpr::Post(
        cpr::Url{endpoint},
        cpr::Body{body.str()},
        cpr::Header{{"Content-Type", "text/plain"}}
    );

    // allocate distance matrix
    dec_t **distances = new dec_t*[words.size()];
    int row = 0;
    int col = 0;

    // parse response to fill distance matrix
    std::istringstream response_ss(r.text);
    for (std::string line; std::getline(response_ss, line); row++) {
        distances[row] = new dec_t[words.size()];

        std::istringstream line_ss(line);
        for (std::string element; std::getline(line_ss, element, ' '); col++) {
            distances[row][col] = std::stof(element);
        }
        col = 0;
    }
    return distances;
}
