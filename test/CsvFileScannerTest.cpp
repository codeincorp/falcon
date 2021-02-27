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