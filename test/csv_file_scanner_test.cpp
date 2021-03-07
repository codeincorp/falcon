#include <gtest/gtest.h>

#include "any_visitor.h"
#include "csv_file_scanner.h"

using namespace std;
using namespace codein;

bool operator==(const Metadata& lhs, const Metadata& rhs);

TEST(CsvFileScannerTests, BasicTest)
{
    Metadata metadata{
        { "a", tiInt },
        { "b", tiFloat },
        { "c", tiString },
    };
    vector<string> lines{
        "1,1.1,John Smith",
        "2,2.2,Alex Smith",
        "3,3.3,Alex Swanson",
    };

    vector<string> names{
        "John Smith",
        "Alex Smith",
        "Alex Swanson",
    };

    std::any empty;

    EXPECT_TRUE(empty == nullany);
    
    auto scanner = makeIterator<CsvFileScanner>("metadata_basic_test.txt", "data_basic_test.csv");
    
    scanner->open();
    size_t i = 0;
    while (scanner->hasMore()) {
        auto row = scanner->processNext();

        if (row == std::nullopt) {
            break;
        }

        EXPECT_TRUE(row.has_value());
        EXPECT_TRUE(row.value().size() == 3);
        EXPECT_EQ(i + 1, any_cast<int>(row.value()[0]));
        EXPECT_FLOAT_EQ((i + 1) * 1.1, any_cast<float>(row.value()[1]));
        EXPECT_EQ(names[i], any_cast<string>(row.value()[2]));

        ++i;
    }

    EXPECT_EQ(i, 3);
}

TEST(CsvFileScannerTests, FileNameConstructorTest)
{
    // Must be same as metadata.txt
    Metadata expectedMetadata{
        { "a", tiInt },
        { "b", tiFloat },
        { "c", tiDouble },
        { "d", tiString },
    };
    // Must be same as data.csv
    vector<string> expectedNames{
        "John Smith",
        "Alex Smith",
        "Alex Swanson",
    };

    auto scanner = makeIterator<CsvFileScanner>("metadata.txt", "data.csv");
    EXPECT_TRUE(scanner->getMetadata() == expectedMetadata);
    
    scanner->open();
    size_t i = 0;
    while (scanner->hasMore()) {
        auto row = scanner->processNext();
        
        if (row == std::nullopt) {
            break;
        }

        EXPECT_TRUE(row.has_value());
        EXPECT_TRUE(row.value().size() == expectedMetadata.size());
        EXPECT_EQ(i + 1, any_cast<int>(row.value()[0]));
        EXPECT_FLOAT_EQ((i + 1) * 1.1f, any_cast<float>(row.value()[1]));
        EXPECT_DOUBLE_EQ((i + 1) * 1.1, any_cast<double>(row.value()[2]));
        EXPECT_EQ(expectedNames[i], any_cast<string>(row.value()[3]));

        ++i;
    }

    EXPECT_EQ(i, 3);
}

TEST(CsvFileScannerTests, FileNameConstructorTest2)
{
    Metadata expectedMetadata1{
        {"product", tiString},
        {"price", tiFloat},
        {"quantity", tiInt},
        {"manufacturer", tiString}

    };

    vector<string> expectedLines{
        "chapaguri, 2.99 , 600, Nongshim",
        "shin Ramyun, 1.99, 450, Nongshim",
        "Jhin Ramyun, 1.89, 777, OTTOGI",
        "Paldo BiBim Myun, 2.10, 280, Paldo"
    };

    vector<vector<any>> expectedFields(expectedLines.size());
    vector<string> fields;
    for (size_t i = 0; i < expectedLines.size(); ++i) {
        fields = parseLine(expectedLines[i]);

        for (size_t k = 0; k < expectedMetadata1.size(); ++k) {
            expectedFields[i].emplace_back(convertTo(
                anyConverters, expectedMetadata1[k].typeIndex, fields[k]
                )
            );
        }
    }

    auto scanner = makeIterator<CsvFileScanner>("metadata1.txt", "data1.csv");
    EXPECT_TRUE(scanner->getMetadata() == expectedMetadata1);
    
    size_t i = 0;
    scanner->open();
    while (scanner->hasMore()) {
        auto row = scanner->processNext();

        if (row == std::nullopt) {
            break;
        }

        const vector<any>& val = row.value();
        EXPECT_TRUE(row.has_value());
        EXPECT_TRUE(val.size() == expectedFields[i].size());
        for (size_t k = 0; k < expectedFields.size() ; ++k) {
            const auto& actualType = val[k].type();
            const auto& expectedType = expectedFields[i][k].type();
            EXPECT_TRUE(actualType == expectedType)
                << "actual type = " << actualType.name() << ", "
                << "expected type = " << expectedType.name();
            EXPECT_TRUE(val[k] == expectedFields[i][k]);
        }
        
        ++i;
    }

    Metadata expectedMetadata2{
        {"num1", tiInt},
        {"num2", tiUint},
        {"presidents", tiString},
        {"num3", tiDouble},
        {"names", tiString}
    };

    vector<vector<any>> expectedFields2{
        {0, 3u, "Adam smith"s, 1.23, "Yoo Jae Suk"s},
        {2, 6u, "george washington"s, 2.46, "Psy"s},
        {4, 9u, "Thomas Jefferson"s, 3.69, "Gideon"s},
        {6, 12u, "Abraham Lincoln"s, 4.92, "Steven"s},
        {8, 15u, "FDR"s, 6.15, "Reid"s}
    };

    scanner = makeIterator<CsvFileScanner>("metadata2.txt", "data2.csv");
    EXPECT_TRUE(scanner->getMetadata() == expectedMetadata2);
    
    i = 0;
    scanner->open();
    while (scanner->hasMore()) {
        auto row = scanner->processNext();

        if (row == std::nullopt) {   
            break;
        }

        EXPECT_TRUE(row.has_value());
        const vector<any>& val = row.value();
        EXPECT_TRUE(val.size() == expectedFields2[i].size());
        for (size_t k = 0; k < 5 ; ++k) {
            const auto& actualType = val[k].type();
            const auto& expectedType = expectedFields2[i][k].type();
            EXPECT_TRUE(actualType == expectedType)
                << "actual type = " << actualType.name() << ", "
                << "expected type = " << expectedType.name();
            EXPECT_TRUE(val[k] == expectedFields2[i][k]);
        }

        ++i;
    }
}

TEST(CsvFileScannerTests, FileNameConstructorFailTests) {
    
    // non-existent file
    EXPECT_THROW(makeIterator<CsvFileScanner>("hello.txt", "Danta.csv"), NonExistentFile);
    EXPECT_THROW(makeIterator<CsvFileScanner>("metadata.txt", "Danta.csv"), NonExistentFile);
    EXPECT_THROW(makeIterator<CsvFileScanner>("hello.txt", "data.csv"), NonExistentFile);

    // empty file
    EXPECT_THROW(makeIterator<CsvFileScanner>("empty_file.txt", "empty_file.csv"), InvalidMetadata);

    // Invalid metadata
    EXPECT_THROW(makeIterator<CsvFileScanner>("empty_fields_metadata.txt", "empty_fields_data.csv"), InvalidMetadata);

    //To Do: Need to deal with a situation when encountered with different data line from metadata
    //Current, CsvFileScanner::processNext() asserts. we can't test the scnario
    // different number of fields

    Metadata expectedMetadata {
        {"pen", tiString},
        {"b", tiInt},
        {"plus", tiUint}
    };

    vector<vector<any>> expectedFields {
        {"pen", -23, 45u, 4},
        {"pencil", -1, 12u},
        {"work", 4, 3u , "plus and minus"},
        {"phone", 23, 15u},
        {"tank", -2, 2u},
        {"scissors", -6, 5u}
    };

    auto scanner = makeIterator<CsvFileScanner>("different_fields.txt", "different_fields.csv");
    scanner->open();
    size_t i = 0;
    while (scanner->hasMore()) {
        auto row = scanner->processNext();

        if (row == std::nullopt) { 
            ++i;  
            continue;
        }

        EXPECT_TRUE(row.has_value());
        const vector<any>& val = row.value();
        EXPECT_TRUE(val.size() == expectedFields[i].size());
        for (size_t k = 0; k < expectedFields.size() ; ++k) {
            const auto& actualType = val[k].type();
            const auto& expectedType = expectedFields[i][k].type();
            EXPECT_TRUE(actualType == expectedType)
                << "actual type = " << actualType.name() << ", "
                << "expected type = " << expectedType.name();
            EXPECT_TRUE(val[k] == expectedFields[i][k]);
        }

        ++i;
    }
    // program should crash
    //EXPECT_THROW(scanner->processNext(), exception);

    // Invalid file format
    EXPECT_THROW(makeIterator<CsvFileScanner>("invalid_metadata.txt", "invalid_data.csv"), InvalidMetadata);
    
}

TEST(CsvFileScannerTests, ConvertToTypeidTest)
{
    EXPECT_EQ(convertToTypeid("int"), tiInt);
    EXPECT_EQ(convertToTypeid("double"), tiDouble);
    EXPECT_EQ(convertToTypeid("string"), tiString);
    EXPECT_EQ(convertToTypeid("float"), tiFloat);
    EXPECT_EQ(convertToTypeid("uint"), tiUint);
    EXPECT_EQ(convertToTypeid("i nt"), tiVoid);
    EXPECT_EQ(convertToTypeid("type"), tiVoid);
    EXPECT_EQ(convertToTypeid("vector"), tiVoid);
}

TEST(CsvFileScannerTests, ParseLineTest) {
    // normal case
    vector<string> expectedLine{
        "field1",
        "field2",
        "field3"
    };
    vector<string> actualLine = parseLine(" field1, field2, field3  ");
    EXPECT_EQ(expectedLine, actualLine);

    // empty line
    vector<string> expectedLine1 {
        ""
    };
    actualLine = parseLine("");
    EXPECT_EQ(expectedLine1, actualLine);

    // space bars only
    vector<string> expectedLine2 {
        ""
    };
    actualLine = parseLine("       ");
    EXPECT_EQ(expectedLine2, actualLine);

    // commas only
    vector<string> expectedLine3 {
        "",
        "",
        "",
        ""
    };
    actualLine = parseLine(", ,,");
    EXPECT_EQ(expectedLine3, actualLine);

    // spaces between 
    vector<string> expectedLine4 {
        "3 34 5",
        "Jan/14 / 2007",
        "location address SSN",
        "1"
    };
    actualLine = parseLine("3 34 5, Jan/14 / 2007 , location address SSN  ,1");
    EXPECT_EQ(expectedLine4, actualLine);

    // tab character
    vector<string> expectedLine5 {
        "tab\t orbit",
        "instructor",
        "computer",
        "calculator"
    };
    actualLine = parseLine("tab\t orbit , instructor,\t\tcomputer,  calculator\t\t");
    EXPECT_EQ(expectedLine5, actualLine);
}

TEST(CsvFileScannerTests, ParseLineMetadataTest)
{
    // true cases
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
    for (size_t i = 0; i < expectedMetadata1.size(); ++i) {
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
    for (size_t i = 0; i < expectedMetadata2.size(); ++i) {
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
    for (size_t i = 0; i < expectedMetadata3.size(); ++i) {
        EXPECT_EQ(expectedMetadata3[i].fieldName, actualMetadata[i].fieldName);
        EXPECT_EQ(expectedMetadata3[i].typeIndex, actualMetadata[i].typeIndex);
    }

    // tab cases
    Metadata expectedMetadata4{
        {"jamMin", tiString},
        {"tori", tiDouble},
        {"Number", tiInt},
    };
    actualMetadata = parseLineMetadata("jamMin\t/string, tori/\tdouble, \tNumber/ int\t\t");
    EXPECT_EQ(expectedMetadata4.size(), actualMetadata.size());
    for (size_t i = 0; i < expectedMetadata4.size(); ++i) {
        EXPECT_EQ(expectedMetadata4[i].fieldName, actualMetadata[i].fieldName);
        EXPECT_EQ(expectedMetadata4[i].typeIndex, actualMetadata[i].typeIndex);
    }

    // Invalid Metadata cases
    // no slash
    EXPECT_THROW(parseLineMetadata("what do you mean"), InvalidMetadata);

    // empty metadata file
    EXPECT_THROW(parseLineMetadata(""), InvalidMetadata);
    EXPECT_THROW(parseLineMetadata("\t\t\t\t "), InvalidMetadata);
        
    // blank spaces only
    EXPECT_THROW(parseLineMetadata("     "), InvalidMetadata);
    EXPECT_THROW(parseLineMetadata("\t\t  \t"),InvalidMetadata);

    // more than one slash
    EXPECT_THROW(parseLineMetadata(" field / int/double"), InvalidMetadata);

    // invalid type name
    EXPECT_THROW(parseLineMetadata("field/i nt"), InvalidMetadata);
    EXPECT_THROW(parseLineMetadata("  field/type  "), InvalidMetadata);
    
    // empty type name
    EXPECT_THROW(parseLineMetadata("onlyField/   "), InvalidMetadata);
    EXPECT_THROW(parseLineMetadata("field1/\t  "), InvalidMetadata);

    // empty field name
    EXPECT_THROW(parseLineMetadata("/int"), InvalidMetadata);
    EXPECT_THROW(parseLineMetadata("       /string"), InvalidMetadata);
    EXPECT_THROW(parseLineMetadata("\t\t\t,string "), InvalidMetadata);
}