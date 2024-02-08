#include <iostream>
#include <string>
#include <unordered_map>

#include "tests.hpp"
#include "../CorpusPeriod.hpp"

int testPeriod() {
    int failed = 0;

    std::cout << std::endl << "GRAPH CONSTRUCTION" << std::endl;
    std::cout << "Node merging" << std::endl;

    std::unordered_map<word_t, std::string> mock_wtostr;
    mock_wtostr[0] = "a";
    mock_wtostr[1] = "b";
    mock_wtostr[2] = "c";

    failed += genericTest("Subword should point to superword", [&mock_wtostr](){
        CorpusPeriod cp = CorpusPeriod({
            // empty docs to avoid ignoring words for occuring too many times
            { 0, 1 }, { 1 }, {}, {}, {}
        }, mock_wtostr);
        cp.constructGraph(1, NULL);
        return cp.wtonode.at(0).word == cp.wtonode.at(1).word;
    });

    failed += genericTest("Chain of subwords should point to common superword", [&mock_wtostr](){
        CorpusPeriod cp = CorpusPeriod({
                { 2, 0, 1 }, { 0, 1 }, { 1 }, {}, {}, {}, {}
        }, mock_wtostr);
        cp.constructGraph(1, NULL);
        return cp.wtonode.at(0).word == 1
            && cp.wtonode.at(1).word == 1
            && cp.wtonode.at(2).word == 1;
    });

    std::cout << "Edge building" << std::endl;

    failed += genericTest("Edges are built without error", [&mock_wtostr](){
        CorpusPeriod cp = CorpusPeriod({
                { 2, 0, 1 }, { 0, 1 }, { 2, 0 }, { 1, 2 }, { 2, 1 }, { 2, 1 }, {}, {}, {}, {}, {}, {}, {} 
        }, mock_wtostr);
        cp.constructGraph(1, NULL);
        return true;
    });

    std::cout << std::endl << "FLOOD WORDS" << std::endl;

    failed += genericTest("Flood words are found based on mean threshold", [&mock_wtostr](){
        CorpusPeriod cp = CorpusPeriod({
                { 0, 1, 1, 2, 2, 2 }, {}, {}, {}
        }, mock_wtostr);
        cp.constructGraph(1, NULL);
        auto non_flood_words = cp.findNonFloodWords(1, 0);
        return non_flood_words.size() == 2
            && non_flood_words[0] == 0
            && non_flood_words[1] == 1;
    });

    failed += genericTest("Flood words are found based on mean and standard deviation threshold", [&mock_wtostr](){
        CorpusPeriod cp = CorpusPeriod({
                { 0, 1, 1, 2, 2, 2 }, {}, {}, {}
        }, mock_wtostr);
        cp.constructGraph(1, NULL);
        auto non_flood_words = cp.findNonFloodWords(1, 2);
        return non_flood_words.size() == 3
            && non_flood_words[0] == 0
            && non_flood_words[1] == 1
            && non_flood_words[2] == 2;
    });

    return failed;
}
