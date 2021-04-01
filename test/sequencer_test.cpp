#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

#include "iterator.h"
#include "metadata.h"
#include "any_visitor.h"
#include "sequencer.h"
#include "mock_scanner.h"
#include "util.h"

using namespace std;
using namespace codein;

TEST(SequencerTests, BasicTest) 
{
    vector<unique_ptr<Iterator>> children;
    Metadata metadata {
        {"a", tiInt}, {"b", tiString}, {"c", tiDouble}
    };

    vector<string> lines1 {
        "1, expression, 1.1",
        "2, paper, 1.2",
        "3, object, 3.2", 
        "4, toilet, 4.1", 
        "5, comptuer, 1.9", 
        "6, headphones, 4.2",
    };

    vector<string> lines2 {
        "-17, mouse, 8.88",
        "-22, cpu, 3.08",
        "-38, wiper,3.18", 
        "-49, books, 37.67",
    };

    vector<string> lines3 {
        "721, output, 9.23",
        "36, tedious, 1.23",
        "534, boring, 2.35",
        "353, noFun, 3.12",
        "56, whatsoever, 4.16",
    };

    children.emplace_back(makeIterator<MockScanner>(metadata, lines1));
    children.emplace_back(makeIterator<MockScanner>(metadata, lines2));
    children.emplace_back(makeIterator<MockScanner>(metadata, lines3));

    vector<vector<any>> expectedFields {
        {1, "expression"s, 1.1},
        {2, "paper"s, 1.2},
        {3, "object"s, 3.2}, 
        {4, "toilet"s, 4.1}, 
        {5, "comptuer"s, 1.9}, 
        {6, "headphones"s, 4.2},
        {-17, "mouse"s, 8.88},
        {-22, "cpu"s, 3.08},
        {-38, "wiper"s,3.18}, 
        {-49, "books"s, 37.67},
        {721, "output"s, 9.23},
        {36, "tedious"s, 1.23},
        {534, "boring"s, 2.35},
        {353, "noFun"s, 3.12},
        {56, "whatsoever"s, 4.16},
    };
    
    auto sequencer = makeIterator<Sequencer>(std::move(children));
    EXPECT_FALSE(sequencer->hasMore());
    verifyIteratorOutput(expectedFields, sequencer);

    sequencer->reopen();
    verifyIteratorOutput(expectedFields, sequencer);
};

TEST (SequencerTests, BasicTest2) 
{
    Metadata metadata {
        {"quantity", tiUint},
        {"grade", tiFloat},
        {"product", tiString}
    };

    vector<string> lines1 { 
        "35, 3.7, productA",
        "42, 3.6, productB",
        "39, 3.9, productC",
        "27, 4.0, productD",
        "51, 3.8, productE",
        "47, 3.72, productF"
    };

    vector<string> lines2 { 
        "21, 3.1, productG",
        "29, 3.6, productH",
        "39, 3.65, productI",
        "32, 3.91, productJ",
        "41, 2.84, productK",
        "37, 3.021, productL"
    };

    vector<string> lines3 { 
        "19, 3.35, productM",
        "42, 3.41, productN",
        "63, 3.29, productO",
        "56, 2.35, productP",
        "48, 2.71, productQ"
    };

    vector<string> lines4 {
        "25, 2.687, productR",
        "45, 3.2, productS"
    };

    vector<unique_ptr<Iterator>> children; 
    children.emplace_back(makeIterator<MockScanner>(metadata, lines1));
    children.emplace_back(makeIterator<MockScanner>(metadata, lines2));
    children.emplace_back(makeIterator<MockScanner>(metadata, lines3));
    children.emplace_back(makeIterator<MockScanner>(metadata, lines4));

    vector<vector<any>> expectedFields {
        {35u, 3.7f, "productA"s},
        {42u, 3.6f, "productB"s},
        {39u, 3.9f, "productC"s},
        {27u, 4.0f, "productD"s},
        {51u, 3.8f, "productE"s},
        {47u, 3.72f, "productF"s},
        {21u, 3.1f, "productG"s},
        {29u, 3.6f, "productH"s},
        {39u, 3.65f, "productI"s},
        {32u, 3.91f, "productJ"s},
        {41u, 2.84f, "productK"s},
        {37u, 3.021f, "productL"s},
        {19u, 3.35f, "productM"s},
        {42u, 3.41f, "productN"s},
        {63u, 3.29f, "productO"s},
        {56u, 2.35f, "productP"s},
        {48u, 2.71f, "productQ"s},
        {25u, 2.687f, "productR"s},
        {45u, 3.2f, "productS"s}
    };

    auto sequencer = makeIterator<Sequencer>(std::move(children));
    EXPECT_FALSE(sequencer->hasMore());
    verifyIteratorOutput(expectedFields, sequencer);
}

TEST (SequencerTests, EmptyOutputFromChildTest) 
{
    Metadata metadata {
        {"a", tiString},
        {"b", tiInt},
        {"c", tiDouble}
    };

    vector<string> lines {};
    vector<string> lines1 {
        "string, 12, 1.1",
        "string1, 13, 1.2",
        "string3, 15, 1.3",
    };
    vector<string> lines2 {};
    vector<string> lines3 {};
    vector<string> lines4 {};

    vector<unique_ptr<Iterator>> children;
    children.emplace_back(makeIterator<MockScanner>(metadata, lines));
    children.emplace_back(makeIterator<MockScanner>(metadata, lines1));
    children.emplace_back(makeIterator<MockScanner>(metadata, lines2));
    children.emplace_back(makeIterator<MockScanner>(metadata, lines3));
    children.emplace_back(makeIterator<MockScanner>(metadata, lines4));

    vector<vector<any>> expectedFields {
        {"string"s, 12, 1.1},
        {"string1"s, 13, 1.2},
        {"string3"s, 15, 1.3}
    };

    auto sequencer = makeIterator<Sequencer>(std::move(children));
    EXPECT_FALSE(sequencer->hasMore());
    verifyIteratorOutput(expectedFields, sequencer);
}

TEST (SequencerTests, DifferentMetadatTest)
{
    Metadata metadata {
        {"a", tiString},
        {"b", tiInt},
        {"c", tiDouble}
    };

    vector<string> lines {
        "string, 12, 1.1",
        "string1, 13, 1.2",
        "string3, 15, 1.3",
        "string4, 21, 2.1"
    };

    Metadata metadata1 {
        {"d", tiUint},
        {"e", tiString}
    };

    vector<string> lines1 {
        "1, what",
        "2, yes",
        "4, note"
    };

    vector<unique_ptr<Iterator>> children;
    children.emplace_back(makeIterator<MockScanner>(metadata, lines));
    children.emplace_back(makeIterator<MockScanner>(metadata1, lines1));

    EXPECT_THROW(makeIterator<Sequencer>(std::move(children)), DiscrepantOutputData);
}
