#include <gtest/gtest.h>
#include <any>
#include <memory>
#include <vector>

#include "any_visitor.h"
#include "iterator.h"
#include "util.h"

using namespace std;
using namespace codein;

void verifyIteratorOutput (
    const vector<vector<any>>& expectedFields, const unique_ptr<Iterator>& iterator)
{
    iterator->open();
    const size_t kExpectedPassLines = expectedFields.size();
    size_t i = 0;

    while (iterator->hasMore()) {
        auto row = iterator->processNext();

        if (row == std::nullopt) {
            break;
        }

        auto val = row.value();
        for (size_t k = 0; k < expectedFields[i].size(); ++k) {
            const auto& actualType = val[k].type();
            const auto& expectedType = expectedFields[i][k].type();
            EXPECT_TRUE(actualType == expectedType)
                << "actual type = " << actualType.name() << ", "
                << "expected type = " << expectedType.name();
            EXPECT_TRUE(val[k] == expectedFields[i][k]);
        }

        ++i;
    }

    EXPECT_FALSE(iterator->hasMore());
    EXPECT_TRUE(iterator->processNext() == std::nullopt);
    EXPECT_EQ(i, kExpectedPassLines);
}
