#include <gtest/gtest.h>

#include "csv_file_scanner.h"
#include "projector.h"

using namespace std;
using namespace codein;

TEST(CsvFileScannerTests, BasicTest)
{
    Metadata metadata{
        { "a", tiInt },
        { "b", tiFloat },
        { "c", tiString },
    };
    vector<string> lines{
        "1,1.1,Yoonsoo Kim",
        "2,2.2,Youngjun Kim",
        "3,3.3,Yeeun Kim",
    };

    vector<string> names{
        "Yoonsoo Kim",
        "Youngjun Kim",
        "Yeeun Kim",
    };

    auto scanner = makeIterator<CsvFileScanner>(metadata, lines);
    
    scanner->open();
    size_t i = 0;
    while (scanner->hasMore()) {
        auto row = scanner->processNext();

        EXPECT_TRUE(row.has_value());
        EXPECT_TRUE(row.value().size() == 3);
        EXPECT_EQ(i + 1, any_cast<int>(row.value()[0]));
        EXPECT_FLOAT_EQ((i + 1) * 1.1, any_cast<float>(row.value()[1]));
        EXPECT_EQ(names[i], any_cast<string>(row.value()[2]));

        ++i;
    }

    EXPECT_EQ(i, 3);
}

TEST(CsvFileScannerTests, ConvertToTypeidTest)
{
    EXPECT_EQ(convertToTypeid("int"), tiInt);
}

TEST(CsvFileScannerTests, ParseLineTest) {
    //normal case
    vector<string> expectedLine{
        "field1",
        "field2",
        "field3"
    };
    vector<string> actualLine = parseLine(" field1, field2, field3  ");
    EXPECT_EQ(expectedLine, actualLine);

    //empty line
    vector<string> expectedLine1 {
        ""
    };
    actualLine = parseLine("");
    EXPECT_EQ(expectedLine1, actualLine);

    //space bars only
    vector<string> expectedLine2 {
        ""
    };
    actualLine = parseLine("       ");
    EXPECT_EQ(expectedLine2, actualLine);

    //commas only
    vector<string> expectedLine3 {
        "",
        "",
        "",
        ""
    };
    actualLine = parseLine(", ,,");
    EXPECT_EQ(expectedLine3, actualLine);

    //spaces between 
    vector<string> expectedLine4 {
        "3 34 5",
        "Jan/14 / 2007",
        "14116 11th DR SE",
        "1"
    };
    actualLine = parseLine("3 34 5, Jan/14 / 2007 , 14116 11th DR SE  ,1");
    EXPECT_EQ(expectedLine4, actualLine);
    EXPECT_FALSE(false);
}

TEST(CsvFileScannerTests, ParseLineMetadataTest)
{
    //true cases
    Metadata expectedMetadata{
        {"abc", tiFloat},
        {"bcd", tiInt}
    };
    auto actualMetadata = parseLineMetadata("abc/float, bcd/int");
    EXPECT_EQ(expectedMetadata.size(), actualMetadata.size());
    for (size_t i = 0; i < expectedMetadata.size(); ++i) {
        EXPECT_EQ(expectedMetadata[i].fieldName, actualMetadata[i].fieldName);
        EXPECT_EQ(expectedMetadata[i].typeIndex, actualMetadata[i].typeIndex);
    }

    Metadata expectedMetadata1{
        {"real number", tiFloat},
        {"quantity", tiInt},
        {"name", tiString}
    };
    actualMetadata = parseLineMetadata("  real number  /float, quantity   /int, name/    string   ");
    EXPECT_EQ(expectedMetadata1.size(), actualMetadata.size());
    for (size_t i = 0; i < expectedMetadata.size(); ++i) {
        EXPECT_EQ(expectedMetadata1[i].fieldName, actualMetadata[i].fieldName);
        EXPECT_EQ(expectedMetadata1[i].typeIndex, actualMetadata[i].typeIndex);
    }

    Metadata expectedMetadata2{
        {"pi", tiDouble},
        {"e", tiDouble},
        {"size", tiUint}
    };
    actualMetadata = parseLineMetadata("pi  /double, e   /double, size/    uint   ");
    EXPECT_EQ(expectedMetadata2.size(), actualMetadata.size());
    for (size_t i = 0; i < expectedMetadata.size(); ++i) {
        EXPECT_EQ(expectedMetadata2[i].fieldName, actualMetadata[i].fieldName);
        EXPECT_EQ(expectedMetadata2[i].typeIndex, actualMetadata[i].typeIndex);
    }

    Metadata expectedMetadata3{
        {"velocity", tiDouble},
        {"x", tiInt},
        {"y", tiInt},
        {"read", tiString}
    };
    actualMetadata = parseLineMetadata(" velocity / double , x   /int, y/int,read/string");
    EXPECT_EQ(expectedMetadata3.size(), actualMetadata.size());
    for (size_t i = 0; i < expectedMetadata.size(); ++i) {
        EXPECT_EQ(expectedMetadata3[i].fieldName, actualMetadata[i].fieldName);
        EXPECT_EQ(expectedMetadata3[i].typeIndex, actualMetadata[i].typeIndex);
    }

    //Invalid Metadata cases

    //no slash

    EXPECT_THROW(parseLineMetadata("what do you mean"), InvalidMetadata);

    //empty metadata file
    EXPECT_THROW(parseLineMetadata(""), InvalidMetadata);
        
    //space bars only
    EXPECT_THROW(parseLineMetadata("     "), InvalidMetadata);

    //more than one slash
    EXPECT_THROW(parseLineMetadata(" field / int/double"), InvalidMetadata);

    //invalid type name
    EXPECT_THROW(parseLineMetadata("field/i nt"), InvalidMetadata);
    EXPECT_THROW(parseLineMetadata("  field/type  "), InvalidMetadata);
    
    //empty type name
    EXPECT_THROW(parseLineMetadata("onlyField/   "), InvalidMetadata);

    //empty field name
    EXPECT_THROW(parseLineMetadata("/int"), InvalidMetadata);
    EXPECT_THROW(parseLineMetadata("       /string"), InvalidMetadata);
        
}
