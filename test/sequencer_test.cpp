#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

#include "iterator.h"
#include "metadata.h"
#include "any_visitor.h"
#include "sequencer.h"
#include "mock_scanner.h"

using namespace std;
using namespace codein;

void compareScannerOutput(const vector<vector<any>>& expectedFields, const unique_ptr<Iterator>& scanner) 
{
    scanner->open();
    const size_t kExpectedPassLines = expectedFields.size();
    Metadata expectedMetadata = scanner->getMetadata();
    size_t i = 0;

    while (scanner->hasMore()) {
        auto row = scanner->processNext();

        if (row == std::nullopt) {
            break;
        }

        auto val = row.value();
        for (size_t k = 0; k < expectedMetadata.size(); ++k) {
            const auto& actualType = val[k].type();
            const auto& expectedType = expectedFields[i][k].type();
            EXPECT_TRUE(actualType == expectedType)
                << "actual type = " << actualType.name() << ", "
                << "expected type = " << expectedType.name();
            EXPECT_TRUE(val[k] == expectedFields[i][k]);
        }

        ++i;
    }

    EXPECT_FALSE(scanner->hasMore());
    EXPECT_EQ(i, kExpectedPassLines);
}

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
        "7, notebook, 3.3",
        "8, phone, 0.1",
        "9, pixel, 3.4",
        "10, google, 7.6",
        "11, keyboard, 6.7",
        "12, desk, 8.8",
        "13, uplift, 4.4",
        "14, cup, 5.5",
        "15, racket, 2.5"
    };

    vector<string> lines2 {
        "-17, mouse, 8.88",
        "-22, cpu, 3.08",
        "-38, wiper,3.18", 
        "-49, books, 37.67",
        "-51, shelf, 56.56",
        "-69, clothes, 11.11",
        "-71, light, 30.0301",
        "-83, owner, 90.09",
        "-92, welcome,3.23",
        "-43, chicken, 3.3333",
        "-68, hungry, .03",
        "-45, ramyun, 13.43",
        "-75, disease, 33.45",
        "-47, colitis, 143.342",
        "-23, pen, 32.3",
        "-91, library, 6.2", 
        "-53, tissue,3.3",
        "-94, body, 1.3",
        "-12, colon, 2.3",
    };

    vector<string> lines3 {
        "721, output, 9.23",
        "36, tedious, 1.23",
        "534, boring, 2.35",
        "353, noFun, 3.12",
        "56, whatsoever, 4.16",
        "4854, calories, 3.74",
        "5632, repetitive, 43.12",
        "4653, tired, 10.343",
        "675, need, 3.3456",
        "3235, vitaminD, 2000.136635",
        "625, nutrition, 1000.5467",
        "544, and, 3432.24525",
        "4623, MSG, 4820.313",
        "5623, and, 1020.3",
        "626, sugar, 7831.4",
        "4646, need, 7450.3",
        "725, energy, 3013.5",
        "3463, accordingTo, 1038.7",
        "523, evolution, 5823.89",
        "4523, by, 1074.4",
        "2342, charles, 10394.5",
        "6724, Darwin, 3421.123"
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
        {7, "notebook"s, 3.3},
        {8, "phone"s, 0.1},
        {9, "pixel"s, 3.4},
        {10, "google"s, 7.6},
        {11, "keyboard"s, 6.7},
        {12, "desk"s, 8.8},
        {13, "uplift"s, 4.4},
        {14, "cup"s, 5.5},
        {15, "racket"s, 2.5},
        {-17, "mouse"s, 8.88},
        {-22, "cpu"s, 3.08},
        {-38, "wiper"s,3.18}, 
        {-49, "books"s, 37.67},
        {-51, "shelf"s, 56.56},
        {-69, "clothes"s, 11.11},
        {-71, "light"s, 30.0301},
        {-83, "owner"s, 90.09},
        {-92, "welcome"s, 3.23},
        {-43, "chicken"s, 3.3333},
        {-68, "hungry"s, .03},
        {-45, "ramyun"s, 13.43},
        {-75, "disease"s, 33.45},
        {-47, "colitis"s, 143.342},
        {-23, "pen"s, 32.3},
        {-91, "library"s, 6.2}, 
        {-53, "tissue"s, 3.3},
        {-94, "body"s, 1.3},
        {-12, "colon"s, 2.3},
        {721, "output"s, 9.23},
        {36, "tedious"s, 1.23},
        {534, "boring"s, 2.35},
        {353, "noFun"s, 3.12},
        {56, "whatsoever"s, 4.16},
        {4854, "calories"s, 3.74},
        {5632, "repetitive"s, 43.12},
        {4653, "tired"s, 10.343},
        {675, "need"s, 3.3456},
        {3235, "vitaminD"s, 2000.136635},
        {625, "nutrition"s, 1000.5467},
        {544, "and"s, 3432.24525},
        {4623, "MSG"s, 4820.313},
        {5623, "and"s, 1020.3},
        {626, "sugar"s, 7831.4},
        {4646, "need"s, 7450.3},
        {725, "energy"s, 3013.5},
        {3463, "accordingTo"s, 1038.7},
        {523, "evolution"s, 5823.89},
        {4523, "by"s, 1074.4},
        {2342, "charles"s, 10394.5},
        {6724, "Darwin"s, 3421.123}
    };
    
    auto scanner = makeIterator<Sequencer>(std::move(children));
    compareScannerOutput(expectedFields, scanner);

    scanner->reopen();
    compareScannerOutput(expectedFields, scanner);
    
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

    auto scanner = makeIterator<Sequencer>(std::move(children));
    compareScannerOutput(expectedFields, scanner);
}

TEST (SequencerTests, differentMetadatTest)
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