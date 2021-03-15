#include <gtest/gtest.h>

#include "metadata.h"
#include "any_visitor.h"
#include "csv_file_scanner.h"
#include "expression.h"

using namespace std;
using namespace codein;

using dataExpected = variant<any, vector<any>, vector<vector<any>>>;

void testLoop (void(*func)(const vector<any>&, const dataExpected&), const string&& metadataFile, 
        const string&& dataFile, const dataExpected& expectedData, const size_t expectedPass, 
        const optional<Metadata>& expectedMetadata = nullopt, const Expression& filterExpr = kAlwaysTrue) {
    auto scanner = makeIterator<CsvFileScanner>(metadataFile, dataFile, filterExpr);
    if (expectedMetadata != nullopt) {
        EXPECT_TRUE(scanner->getMetadata() == expectedMetadata.value());
    }
    scanner->open();
    const size_t kExpectedPassLines = expectedPass;
    size_t i = 0;

    while (scanner->hasMore()) {
        try {
            auto row = scanner->processNext();
            if (row == std::nullopt) {
                break;
            }

            EXPECT_TRUE(row.has_value());
            if (holds_alternative<vector<vector<any>>>(expectedData)) {
                auto expectedFields = get<2>(expectedData);
                func(row.value(), expectedFields[i]);
            }
            else {
                func(row.value(), expectedData);
            }

            ++i;
        }
        catch(WrongMetadata e) {
            cerr << "WrongMetadata thrown\n";
            break;
        }
    }

    EXPECT_EQ(i, kExpectedPassLines);
}

void(*check)(const vector<any>&, const dataExpected&) = [](const vector<any>& val, const dataExpected& expectedData) {
    auto expectedFields = get<1>(expectedData);
    EXPECT_EQ(val.size(), expectedFields.size());
    for (size_t k = 0; k < expectedFields.size() ; ++k) {
        const auto& actualType = val[k].type();
        const auto& expectedType = expectedFields[k].type();
        EXPECT_TRUE(actualType == expectedType)
            << "actual type = " << actualType.name() << ", "
            << "expected type = " << expectedType.name();
        EXPECT_TRUE(val[k] == expectedFields[k]);
    }
};

TEST(CsvFileScannerTests, filterTest) 
{
    // a == 2u
    Expression filterExpr {
        .opCode = OpCode::Eq,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(2u)},
        }
    };

    testLoop([](const vector<any>& actual, const dataExpected& expected){ 
        EXPECT_TRUE(get<0>(expected) == actual[0]); },
        "fileScanner_filter_test.txt", "fileScanner_filter_test.csv", any(2u), 8, nullopt, filterExpr 
    );
}

TEST(CsvFileScannerTests, filterTest1) 
{
    // b >= 10
    Expression filterExpr {
        .opCode = OpCode::Gte,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("b"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(10)},
        }
    };

    testLoop([](const vector<any>& actual, const dataExpected& expected){ 
        EXPECT_TRUE(actual[1] >= get<0>(expected)); },
        "fileScanner_filter_test.txt", "fileScanner_filter_test.csv", any(10), 7, nullopt, filterExpr 
    );
}

TEST(CsvFileScannerTests, filterTest2)
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

    vector<any> expectedData {1u, 10};
    testLoop([](const vector<any>& actual, const dataExpected& expected) { 
        EXPECT_TRUE(get<1>(expected)[0] == actual[0] );
        EXPECT_TRUE(actual[1] >= get<1>(expected)[1]); },
        "fileScanner_filter_test.txt", "fileScanner_filter_test.csv", expectedData, 2, nullopt, filterExpr 
    );
}

TEST(CsvFileScannerTests, filterTest3) 
{
    // d == "OTTOGI"
    Expression filterExpr {
        .opCode = OpCode::Eq,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("d"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any("OTTOGI"s)},
        }
    };

    testLoop([](const vector<any>& actual, const dataExpected& expected) {EXPECT_TRUE(get<0>(expected) == actual[3]);},
        "fileScanner_filter_test.txt", "fileScanner_filter_test.csv", any("OTTOGI"s), 3, nullopt, filterExpr 
    );
}

TEST(CsvFileScannerTests, filterTest4) 
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

    testLoop([](const vector<any>& actual, const dataExpected& expected) {EXPECT_TRUE(get<0>(expected) == actual[4]);},
        "fileScanner_filter_test.txt", "fileScanner_filter_test.csv", any("OTTOGI1"s), 3, nullopt, filterExpr 
    );
}

TEST(CsvFileScannerTests, nonExistentFieldNameTest) 
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
    scanner->open();
    EXPECT_THROW(scanner->processNext(), UnknownName);
}

TEST(CsvFileScannerTests, noPassFilterTest) 
{
    // c < 0
    Expression filterExpr {
        .opCode = OpCode::Lt,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("c"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(0)}, 
        }
    };

    testLoop([](const vector<any>& actual, const dataExpected& expected) {}, 
        "fileScanner_filter_test.txt", "fileScanner_filter_test.csv", any(0), 0, nullopt, filterExpr 
    );
}

TEST(CsvFileScannerTests, BasicTest)
{
    Metadata metadata{
        { "a", tiInt },
        { "b", tiFloat },
        { "c", tiString },
    };

    vector<vector<any>> expectedLines{
        {1, 1.1f, "John Smith"s},
        {2, 2.2f, "Alex Smith"s},
        {3, 3.3f, "Alex Swanson"s}
    };

    testLoop(check, "metadata_basic_test.txt", "data_basic_test.csv", expectedLines, expectedLines.size(), metadata );
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
    vector<vector<any>> expectedLines {
        {1,1.1f, 1.1, "John Smith"s},
        {2,2.2f, 2.2, "Alex Smith"s},
        {3,3.3f, 3.3, "Alex Swanson"s},
    };

    testLoop(check, "metadata.txt", "data.csv", expectedLines, expectedLines.size(), expectedMetadata);
}

TEST(CsvFileScannerTests, FileNameConstructorTest2)
{
    Metadata expectedMetadata1{
        {"product", tiString},
        {"price", tiFloat},
        {"quantity", tiInt},
        {"manufacturer", tiString}

    };

    vector<vector<any>> expectedFields{
        {"chapaguri"s, 2.99f , 600, "Nongshim"s},
        {"shin Ramyun"s, 1.99f, 450, "Nongshim"s},
        {"Jhin Ramyun"s, 1.89f, 777, "OTTOGI"s},
        {"Paldo BiBim Myun"s, 2.10f, 280, "Paldo"s}
    };

    testLoop(check, "metadata1.txt", "data1.csv", expectedFields, expectedFields.size(), expectedMetadata1);

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

    testLoop(check, "metadata2.txt", "data2.csv", expectedFields2, expectedFields2.size(), expectedMetadata2);
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
    testLoop(check, "different_fields.txt", "different_fields.csv", expectedFields, expectedFields.size(), expectedMetadata);
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
    testLoop(check, "different_fields2.txt", "different_fields2.csv", expectedFields2, expectedFields2.size(), expectedMetadata2);
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
    testLoop(check, "different_fields3.txt", "different_fields3.csv", expectedFields3, 26, expectedMetadata3);
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

void checkLine(const vector<string>& expectedLine, string&& realLine) {
    vector<string> actualLine = parseLine(realLine);
    EXPECT_EQ(expectedLine, actualLine);
}

TEST(CsvFileScannerTests, ParseLineTest) {
    vector<vector<string>> expectedLines {
        // normal case
        {"field1","field2","field3"},
        // empty line
        {""},
        // space bars only
        {""},
        // commas only
        {"", "", "", ""},
        // spaces between
        {"3 34 5", "Jan/14 / 2007", "location address SSN", "1"},
        // tab character
        {"tab\t orbit", "instructor", "computer", "calculator"}
    };

    checkLine(expectedLines[0], " field1, field2, field3  ");
    checkLine(expectedLines[1], "");
    checkLine(expectedLines[2], "       ");
    checkLine(expectedLines[3], ", ,,");
    checkLine(expectedLines[4], "3 34 5, Jan/14 / 2007 , location address SSN  ,1");
    checkLine(expectedLines[5], "tab\t orbit , instructor,\t\tcomputer,  calculator\t\t");
}

void checkMetadata(Metadata& expectedMetadata, string&& line) {
    auto actualMetadata = parseLineMetadata(line);
    EXPECT_EQ(expectedMetadata.size(), actualMetadata.size());
    for (size_t i = 0; i < expectedMetadata.size(); ++i) {
        EXPECT_EQ(expectedMetadata[i].fieldName, actualMetadata[i].fieldName);
        EXPECT_EQ(expectedMetadata[i].typeIndex, actualMetadata[i].typeIndex);
    }
}

TEST(CsvFileScannerTests, ParseLineMetadataTest)
{
    // true cases
    vector<Metadata> expectedMetadata {
        {{"abc", tiFloat},{"bcd", tiInt}},
        {{"real number", tiFloat}, {"quantity", tiInt}, {"name", tiString}},
        {{"pi", tiDouble}, {"e", tiDouble}, {"size", tiUint}},
        {{"velocity", tiDouble}, {"x", tiInt}, {"y", tiInt}, {"read", tiString}},
        {{"jamMin", tiString}, {"tori", tiDouble}, {"Number", tiInt}}
    };

    checkMetadata(expectedMetadata[0], "abc/float, bcd/int");
    checkMetadata(expectedMetadata[1], "  real number  /float, quantity   /int, name/    string   ");
    checkMetadata(expectedMetadata[2], "pi  /double, e   /double, size/    uint   ");
    checkMetadata(expectedMetadata[3], " velocity / double , x   /int, y/int,read/string");
    checkMetadata(expectedMetadata[4], "jamMin\t/string, tori/\tdouble, \tNumber/ int\t\t");

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