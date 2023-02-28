#include <iostream>
#include <string>
#include <unordered_map>

#include "tests.hpp"
#include "../CorpusPeriod.hpp"

int testPeriod() {
    int failedTests = 0;

    std::cout << std::endl << "GRAPH CONSTRUCTION" << std::endl;
    std::cout << "Node merging" << std::endl;

    std::unordered_map<word_t, std::string> mock_wtostr;
    mock_wtostr[0] = "a";
    mock_wtostr[1] = "b";
    mock_wtostr[2] = "c";

    failedTests += genericTest("Subword should point to superword", [&mock_wtostr](){
        CorpusPeriod cp = CorpusPeriod({
            // empty docs to avoid ignoring words for occuring too many times
            { 0, 1 }, { 1 }, {}, {}, {}
        }, mock_wtostr, 1);
        return cp.wtonode.at(0)->word == cp.wtonode.at(1)->word;
    });

    failedTests += genericTest("Chain of subwords should point to common superword", [&mock_wtostr](){
        CorpusPeriod cp = CorpusPeriod({
                { 2, 0, 1 }, { 0, 1 }, { 1 }, {}, {}, {}, {}
        }, mock_wtostr, 1);
        return cp.wtonode.at(0)->word == 1
            && cp.wtonode.at(1)->word == 1
            && cp.wtonode.at(2)->word == 1;
    });

    std::cout << "Edge building" << std::endl;

    failedTests += genericTest("Edges are built without error", [&mock_wtostr](){
        CorpusPeriod cp = CorpusPeriod({
                { 2, 0, 1 }, { 0, 1 }, { 2, 0 }, { 1, 2 }, { 2, 1 }, { 2, 1 }, {}, {}, {}, {}, {}, {}, {} 
        }, mock_wtostr, 1);
        return true;
    });

    std::cout << std::endl << "FLOOD WORDS" << std::endl;

    failedTests += genericTest("Flood words are found based on mean threshold", [&mock_wtostr](){
        CorpusPeriod cp = CorpusPeriod({
                { 0, 1, 1, 2, 2, 2 }, {}, {}, {}
        }, mock_wtostr, 1);
        auto nonFloodWords = cp.findNonFloodWords(1, 0);
        return nonFloodWords.size() == 2
            && nonFloodWords[0] == 0
            && nonFloodWords[1] == 1;
    });

    failedTests += genericTest("Flood words are found based on mean and standard deviation threshold", [&mock_wtostr](){
        CorpusPeriod cp = CorpusPeriod({
                { 0, 1, 1, 2, 2, 2 }, {}, {}, {}
        }, mock_wtostr, 1);
        auto nonFloodWords = cp.findNonFloodWords(1, 2);
        return nonFloodWords.size() == 3
            && nonFloodWords[0] == 0
            && nonFloodWords[1] == 1
            && nonFloodWords[2] == 2;
    });

    return failedTests;
}
