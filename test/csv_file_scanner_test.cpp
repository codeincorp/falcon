#include <gtest/gtest.h>

#include "metadata.h"
#include "any_visitor.h"
#include "csv_file_scanner.h"
#include "to_any_converter.h"
#include "util.h"

using namespace std;
using namespace codein;

TEST(CsvFileScannerTests, ConvertToTypeidTest)
{
    EXPECT_EQ(convertToTypeid("int"), tiInt);
    EXPECT_EQ(convertToTypeid("bool"), tiBool);
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
    
    auto scanner = makeIterator<CsvFileScanner>("metadata_basic_test.txt", "data_basic_test.csv");
    
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
            expectedFields[i].emplace_back(convertTo(expectedMetadata1[k].typeIndex, fields[k])
            );
        }
    }

    auto scanner = makeIterator<CsvFileScanner>("metadata1.txt", "data1.csv");
    EXPECT_TRUE(scanner->getMetadata() == expectedMetadata1);
    
    size_t i = 0;
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

    // Invalid file format
    EXPECT_THROW(makeIterator<CsvFileScanner>("invalid_metadata.txt", "invalid_data.csv"), InvalidMetadata);
}

/* total number of line is 41. however, as readLines_ goes over 30, 
 * there will be too many error lines and processNext() will throw WrongMetadata 
 * at line 31 before reading the entire file.
 */
TEST(CsvFileScannerTests, InvalidLinesInFileTest1) {
    // Invalid line(s)
    Metadata expectedMetadata{
        {"pen", tiString},
        {"b", tiInt},
        {"plus", tiUint}
    };

    // includes only correct lines
    vector<vector<any>> expectedFields {
        {"pencil"s, -1, 12u},
        {"phone"s, 23, 15u},
        {"tank"s, -2, 2u},
        {"scissors"s, -6, 5u},
        {"mouse"s, 3, 4u},
        {"er"s,-35,12u},
        {"line14"s, 14, 14u},
        {"line17"s, 17, 18u},
        {"line18"s, 90,91u},
        {"line22"s, 22, 23u},
        {"line25"s, 25,25u},
        {"line27"s, 27,27u},
        {"line29"s, 14, 14u},
        {"line30"s, 234, 3u}
    };

    auto scanner = makeIterator<CsvFileScanner>("different_fields.txt", "different_fields.csv");
    EXPECT_TRUE(scanner->getMetadata() == expectedMetadata);
    const size_t kValidLinesInDataFile = 14;
    size_t i = 0;

    while (scanner->hasMore()) {
        // 14 = number of correct lines in the file
        if (i == 14) {
            EXPECT_THROW(scanner->processNext(), WrongMetadata);
            break;
        }
        auto row = scanner->processNext();

        // processNext will skip wrong lines and return next valid line
        EXPECT_TRUE(row.has_value());
        const vector<any>& val = row.value();
        EXPECT_TRUE(val.size() == expectedFields[i].size());
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
    EXPECT_EQ(i, kValidLinesInDataFile);
}

/* different_fields2.csv has 9 total invalid lines which is more than half of the entire lines. 
 * However, since the number of total lines is below 30, processNext should only print out error message 
 * without aborting the process and throwing WrongMetadata
 */
TEST(CsvFileScannerTests, InvalidLinesInFileTest2) {
    Metadata expectedMetadata2 {
        {"a", tiFloat},
        {"clown", tiString},
        {"mouse", tiInt}
    };

    vector<vector<any>> expectedFields2 {
        {5.12f, "gary"s, 2},
        {6.43f, "computer"s, 5},
        {43.1f, "age"s, 10},
        {9.87f, "device"s, 13},
        {8.13f, "laptop"s, 67},
        {2.91f, "note"s, 23},
        {4.84f, "write"s, 1},
        {3.33f, "medicine"s, 90}
    };

    auto scanner = makeIterator<CsvFileScanner>("different_fields2.txt", "different_fields2.csv");
    EXPECT_TRUE(scanner->getMetadata() == expectedMetadata2);
    const size_t kValidLinesInDataFile1 = 8;
    size_t i = 0;

    while (scanner->hasMore()) {
        auto row = scanner->processNext();

        if (row == std::nullopt) {
            break;
        }

        EXPECT_TRUE(row.has_value());
        const vector<any>& val = row.value();
        EXPECT_TRUE(val.size() == expectedFields2[i].size());
        for (size_t k = 0; k < expectedMetadata2.size(); ++k) {
            const auto& actualType = val[k].type();
            const auto& expectedType = expectedFields2[i][k].type();
            EXPECT_TRUE(actualType == expectedType)
                << "actual type = " << actualType.name() << ", "
                << "expected type = " << expectedType.name();
            EXPECT_TRUE(val[k] == expectedFields2[i][k]);
        }

        ++i;
    }
    EXPECT_EQ(i, kValidLinesInDataFile1);
}

/* In the case 1, further reading process should be aborted because error lines are more than half
 * the total read lines in this case, the loop should read the entire file and print out how many lines 
 * were invalid as the number of error lines never go beyond half of the total read lines
 */
TEST(CsvFileScannerTests, InvalidLinesInFileTest3) {
    Metadata expectedMetadata3 {
        {"one", tiString},
        {"two", tiInt},
        {"three", tiUint},
        {"four", tiString}
    };

    vector<any> expectedFields3 {
        "correct"s, 1, 1u, "correct"s
    };

    auto scanner = makeIterator<CsvFileScanner>("different_fields3.txt", "different_fields3.csv");
    EXPECT_TRUE(scanner->getMetadata() == expectedMetadata3);
    const size_t kValidLinesInDataFile2 = 26;
    size_t i = 0;

    while (scanner->hasMore()) {
        auto row = scanner->processNext();

        if (row == std::nullopt) {
            break;
        }

        EXPECT_TRUE(row.has_value());
        const vector<any>& val = row.value();
        EXPECT_TRUE(val.size() == expectedFields3.size());
        for (size_t k = 0; k < expectedMetadata3.size(); ++k) {
            const auto& actualType = val[k].type();
            const auto& expectedType = expectedFields3[k].type();
            EXPECT_TRUE(actualType == expectedType)
                << "actual type = " << actualType.name() << ", "
                << "expected type = " << expectedType.name();
            EXPECT_TRUE(val[k] == expectedFields3[k]);
        }

        ++i;
    }
    EXPECT_EQ(i,kValidLinesInDataFile2);
}

TEST(CsvFileScannerTests, FilterTest) 
{
    // a == 2u
    Expression filterExpr {
        .opCode = OpCode::Eq,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(2u)},
        }
    };

    auto scanner = makeIterator<CsvFileScanner>("fileScanner_filter_test.txt", "fileScanner_filter_test.csv", filterExpr);

    while (scanner->hasMore()) {
        auto row = scanner->processNext();

        if (row == std::nullopt) {
            break;
        }

        EXPECT_EQ(2u, any_cast<unsigned int>(row.value()[0]));
    }
 
}

TEST(CsvFileScannerTests, FilterTest1) 
{
    // b >= 10
    Expression filterExpr {
        .opCode = OpCode::Gte,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("b"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(10)},
        }
    };

    const size_t kExpectedPassedLines = 7;
    auto scanner = makeIterator<CsvFileScanner>("fileScanner_filter_test.txt", "fileScanner_filter_test.csv", filterExpr);
    size_t i = 0;

    while (scanner->hasMore()) {
        auto row = scanner->processNext();

        if (row == std::nullopt) {
            break;
        }

        EXPECT_TRUE(10 <= any_cast<int>(row.value()[1]));
        i++;
    }

    EXPECT_EQ(i,kExpectedPassedLines);
}

TEST(CsvFileScannerTests, FilterTest2)
{
    // a == 1u && b >= 10
    Expression filterExpr {
        .opCode = OpCode::And,

        .leafOrChildren = vector<Expression>{
            {
                // a == 1u
                .opCode = OpCode::Eq, 
                .leafOrChildren = vector<Expression>{
                    {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
                    {.opCode = OpCode::Const, .leafOrChildren = std::any(1u)},
                }
            },
           
            {
                // b >= 10
                .opCode = OpCode::Gte, 
                .leafOrChildren = vector<Expression>{
                    {.opCode = OpCode::Ref, .leafOrChildren = std::any("b"s)},
                    {.opCode = OpCode::Const, .leafOrChildren = std::any(10)},
                }
            },
        }
    };

    const size_t kExpectedPassedLines = 2;
    auto scanner = makeIterator<CsvFileScanner>("fileScanner_filter_test.txt", "fileScanner_filter_test.csv", filterExpr);
    size_t i = 0;

    while (scanner->hasMore()) {
        auto row = scanner->processNext();

        if (row == std::nullopt) {
            break;
        }

        EXPECT_EQ(1u, any_cast<unsigned int>(row.value()[0]));
        EXPECT_GE(any_cast<int>(row.value()[1]), 10);
        i++;
    }

    EXPECT_EQ(i, kExpectedPassedLines);
}

TEST(CsvFileScannerTests, FilterTest3) 
{
    // d == "OTTOGI"
    Expression filterExpr {
        .opCode = OpCode::Eq,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("d"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any("OTTOGI"s)},
        }
    };

    const size_t kExpectedPassedLines = 3;
    auto scanner = makeIterator<CsvFileScanner>("fileScanner_filter_test.txt", "fileScanner_filter_test.csv", filterExpr);
    size_t i = 0;

    while (scanner->hasMore()) {
        auto row = scanner->processNext();

        if (row == std::nullopt) {
            break;
        }

        EXPECT_EQ("OTTOGI", any_cast<string>(row.value()[3]));
        i++;
    }

    EXPECT_EQ(i , kExpectedPassedLines);
}

TEST(CsvFileScannerTests, FilterTest4) 
{
    // e(string) == d(string) + "1"
    Expression filterExpr {
        .opCode = OpCode::Eq,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("e"s)},
            {
                .opCode = OpCode::Add, 
                .leafOrChildren = vector<Expression>{
                    {.opCode = OpCode::Ref, .leafOrChildren = std::any("d"s)},
                    {.opCode = OpCode::Const, .leafOrChildren = std::any("1"s)},
                }
            },
            
        }
    };

    const size_t kExpectedPassedLines = 3;
    auto scanner = makeIterator<CsvFileScanner>("fileScanner_filter_test.txt", "fileScanner_filter_test.csv", filterExpr);
    size_t i = 0;

    while (scanner->hasMore()) {
        auto row = scanner->processNext();

        if (row == std::nullopt) {
            break;
        }

        EXPECT_EQ("OTTOGI1", any_cast<string>(row.value()[4]));
        i++;
    }

    EXPECT_EQ(i, kExpectedPassedLines);
}

TEST(CsvFileScannerTests, NonExistentFieldNameTest) 
{
    // calling non-existent field name for expression, should throw UnknownName exception
    Expression filterExpr {
        .opCode = OpCode::Eq,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any(":)"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(2u)},
        }
    };

    auto scanner = makeIterator<CsvFileScanner>("fileScanner_filter_test.txt", "fileScanner_filter_test.csv", filterExpr);
    EXPECT_THROW(scanner->processNext(), UnknownName);
}

TEST(CsvFileScannerTests, NoPassFilterTest) 
{
    // c < 0
    Expression filterExpr {
        .opCode = OpCode::Lt,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("c"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(0)}, 
        }
    };

    const size_t kExpectedPassedLines = 0;
    auto scanner = makeIterator<CsvFileScanner>("fileScanner_filter_test.txt", "fileScanner_filter_test.csv", filterExpr);
    size_t i = 0;

    while (scanner->hasMore()) {
        auto row = scanner->processNext();
        EXPECT_TRUE(row == std::nullopt);

        if (row == std::nullopt) {
            break;
        }

        i++;
    }

    EXPECT_EQ(i, kExpectedPassedLines);
}

TEST(CsvFileScannerTests, ProjectionsTest)
{
    vector<vector<any>> expectedFields {
        {1u, 3},
        {2u, 2},
        {2u, 3},
        {1u, 1},
        {2u, 1},
        {2u, 1},
        {1u, 3},
        {2u, 3},
        {1u, 3},
        {1u,3},
        {2u,3},
        {2u,5},
        {2u,4},
    };

    vector<Expression> projections {
        {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
        {.opCode = OpCode::Ref, .leafOrChildren = std::any("c"s)},
    };

    verifyIteratorOutput(expectedFields, 
        makeIterator<CsvFileScanner>("fileScanner_filter_test.txt", "fileScanner_filter_test.csv", kAlwaysTrue, projections)
    );
}

TEST(CsvFileScannerTests, ProjectionsTest2) {
    vector<vector<any>> expectedFields {
        {1u, 3, "OTTOGI"s},
        {2u, 2, "Nongshim"s},
        {2u, 3, "Paldo"s},
        {1u, 1, "Samyang"s},
        {2u, 1, "Paldo"s},
        {2u, 1, "Nongshim"s},
        {1u, 3, "Paldo"s},
        {2u, 3, "Nongshim"s},
        {1u, 3, "Samyang"s},
        {1u, 3, "Samyang"s},
        {2u, 3, "OTTOGI"s},
        {2u, 5, "OTTOGI"s},
        {2u, 4, "Samyang"s},
    };

    vector<Expression> projections {
        {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
        {.opCode = OpCode::Ref, .leafOrChildren = std::any("c"s)},
        {.opCode = OpCode::Ref, .leafOrChildren = std::any("d"s)},
    };

    verifyIteratorOutput(expectedFields, 
        makeIterator<CsvFileScanner>("fileScanner_filter_test.txt", "fileScanner_filter_test.csv", kAlwaysTrue, projections)
    );
    
    expectedFields = {
        {2u, 2, "Nongshim"s},
        {2u, 3, "Paldo"s},
        {2u, 1, "Paldo"s},
        {2u, 1, "Nongshim"s},
        {2u, 3, "Nongshim"s},
        {2u, 3, "OTTOGI"s},
        {2u, 5, "OTTOGI"s},
        {2u, 4, "Samyang"s},
    };

    // a == 2u
    Expression filterExpr {
        .opCode = OpCode::Eq,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(2u)},
        }
    };

    verifyIteratorOutput(expectedFields, 
        makeIterator<CsvFileScanner>("fileScanner_filter_test.txt", "fileScanner_filter_test.csv", filterExpr, projections)
    );
}

TEST(CsvFileScannerTests, ProjectionsTest3) 
{
    // d == OTTOGI || e == Nongshim
    Expression filterExpr {
        .opCode = OpCode::Or, 
        .leafOrChildren = vector<Expression> {
            // d == OTTOGI
            {
                .opCode = OpCode::Eq, 
                .leafOrChildren = vector<Expression> {
                    {.opCode = OpCode::Ref, .leafOrChildren = any("d"s)},
                    {.opCode = OpCode::Const, .leafOrChildren = any("OTTOGI"s)}
                }
            },
            // e == Nongshim
            {
                .opCode = OpCode::Eq, 
                .leafOrChildren = vector<Expression> {
                    {.opCode = OpCode::Ref, .leafOrChildren = any("e"s)},
                    {.opCode = OpCode::Const, .leafOrChildren = any("Nongshim"s)}
                }
            },
        }
    };

    vector<Expression> projections {
        //  b - c
        {
            .opCode = OpCode::Sub, 
            .leafOrChildren = vector<Expression> {
                {.opCode = OpCode::Ref, .leafOrChildren = any("b"s)},
                {.opCode = OpCode::Ref, .leafOrChildren = any("c"s)}
            }
        },
        // d
        {.opCode = OpCode::Ref, .leafOrChildren = any("d"s)},
        // b >= 10
        {
            .opCode= OpCode::Gte, 
            .leafOrChildren = vector<Expression> {
                {.opCode = OpCode::Ref, .leafOrChildren = any("b"s)},
                {.opCode = OpCode::Const, .leafOrChildren = any(10)}
            }
        }
    };

    vector<vector<any>> expectedFields {
        {6, "OTTOGI"s, false},
        {5, "Nongshim"s, false},
        {2, "Nongshim"s, false},
        {38, "Nongshim"s, true},
        {5, "OTTOGI"s, false},
        {5, "OTTOGI"s, true},
    };

    verifyIteratorOutput(expectedFields, 
        makeIterator<CsvFileScanner>("fileScanner_filter_test.txt", "fileScanner_filter_test.csv", filterExpr, projections)
    );
}

TEST(CsvFileScannerTests, UnKnownProjectionTest)
{
    // :)
    vector<Expression> projections {
        {.opCode = OpCode::Ref, .leafOrChildren = any(":)"s)},
        {
            // a + 10u
            .opCode = OpCode::Add, 
            .leafOrChildren = vector<Expression> {
                {.opCode = OpCode::Ref, .leafOrChildren = any("a"s)},
                {.opCode = OpCode::Const, .leafOrChildren = any(10u)}
            }
        }
    };

    auto scanner = makeIterator<CsvFileScanner>("fileScanner_filter_test.txt", "fileScanner_filter_test.csv", kAlwaysTrue, projections);
        
    EXPECT_THROW(scanner->processNext(), UnknownName);
}

TEST(CsvFileScannerTests, UnsupportedOperationProjectionsTest)
{
    vector<Expression> projections {
        {.opCode = OpCode::Ref, .leafOrChildren = any("a"s)},
        {
            // d(string) + c(int)
            .opCode = OpCode::Add, 
            .leafOrChildren = vector<Expression> {
                {.opCode = OpCode::Ref, .leafOrChildren = any("d"s)},
                {.opCode = OpCode::Ref, .leafOrChildren = any("c"s)}
            }
        }
    };

    auto scanner = makeIterator<CsvFileScanner>("fileScanner_filter_test.txt", "fileScanner_filter_test.csv", kAlwaysTrue, projections);

    EXPECT_THROW(scanner->processNext(), UnsupportedOperation);
}

TEST(CsvFileScannerTests, UnorderedProjectionsTest)
{
    vector<vector<any>> expectedFields {
        {"OTTOGI"s, 3, 1u},
        {"Nongshim"s, 2, 2u},
        {"Paldo"s, 3, 2u},
        {"Samyang"s, 1, 1u},
        {"Paldo"s, 1, 2u},
        {"Nongshim"s, 1, 2u},
        {"Paldo"s, 3, 1u},
        {"Nongshim"s, 3, 2u},
        {"Samyang"s, 3, 1u},
        {"Samyang"s, 3, 1u},
        {"OTTOGI"s, 3, 2u},
        {"OTTOGI"s, 5, 2u},
        {"Samyang"s, 4, 2u}
    };

    vector<Expression> projections {
        {.opCode = OpCode::Ref, .leafOrChildren = std::any("d"s)},
        {.opCode = OpCode::Ref, .leafOrChildren = std::any("c"s)},
        {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)}
    };

    verifyIteratorOutput(expectedFields, 
        makeIterator<CsvFileScanner>("fileScanner_filter_test.txt", "fileScanner_filter_test.csv", kAlwaysTrue, projections)
    );
}
