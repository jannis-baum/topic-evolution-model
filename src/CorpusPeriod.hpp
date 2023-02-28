#ifndef CORPUS_PERIOD_HPP
#define CORPUS_PERIOD_HPP

#include <initializer_list>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <vector>

#include "Document.hpp"
#include "SemanticGraph.hpp"
#include "types.hpp"

class CorpusPeriod {
    // reference to Corpus' mapping to resolve word_t (aka int) to string
    const std::unordered_map<word_t, std::string> &wtostr;
    // map words that exist in graph to nodes
    std::unordered_map<word_t, SemanticNode> wtonodeAll;
    std::vector<Document> documents;

    // number of documents that contain all given words
    int nDocumentsContaining(const std::initializer_list<word_t> words) const;
    // asymmetric term correlation, see definitions.md or paper
    // (will be nullopt if no co-occurrence)
    std::optional<dec_t> termCorrelation(const word_t k, const word_t z) const;

    void constructGraph(const dec_t delta);
    void constructNodes();
    void addEdges(const dec_t delta);

    public:
        // maps words to their assigned node
        // public for testing
        std::unordered_map<word_t, SemanticNode *> wtonode;

        CorpusPeriod(
            const std::vector<std::vector<word_t>> structuredDocuments,
            const std::unordered_map<word_t, std::string> &wtostr,
            const dec_t delta);
        CorpusPeriod(
            const std::vector<Document> documents,
            const std::unordered_map<word_t, std::string> &wtostr,
            const dec_t delta);
        std::vector<word_t> findNonFloodWords(const dec_t c, const dec_t alpha) const;
        // see definitions.md or paper
        virtual dec_t nutrition(const word_t word, const dec_t c) const;

        // streaming (e.g. printing) operator <<
        friend std::ostream& operator<<(std::ostream& os, CorpusPeriod const &period) {
            for (const auto document : period.documents) os << document << std::endl;
            return os;
        };
};

#endif
