#ifndef CORPUS_PERIOD_HPP
#define CORPUS_PERIOD_HPP

#include <initializer_list>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "Document.hpp"
#include "SemanticGraph.hpp"
#include "types.hpp"

class CorpusPeriod {
    // reference to Corpus' mapping to resolve word_t (aka int) to string
    const std::unordered_map<word_t, std::string> &wtostr;
    const std::vector<Document> documents;

    public:
        // map words that exist in graph to nodes
        std::unordered_map<word_t, SemanticNode> wtonode;

        CorpusPeriod(const std::vector<Document> documents, const std::unordered_map<word_t, std::string> &wtostr);
        // see definitions.md or paper
        dec_t nutrition(const word_t word, const dec_t c) const;
        // number of documents that contain all given words
        int nDocumentsContaining(const std::initializer_list<word_t> words) const;
        // asymmetric term correlation, see definitions.md or paper
        dec_t termCorrelation(const word_t k, const word_t z) const;
        // semantic graph as adjacency list
        std::vector<SemanticNode> semanticGraph(const dec_t delta) const;

        // streaming (e.g. printing) operator <<
        friend std::ostream& operator<<(std::ostream& os, CorpusPeriod const &period) {
            for (const auto document : period.documents) os << document << std::endl;
            return os;
        };
};

#endif
