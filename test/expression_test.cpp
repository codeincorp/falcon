#include <gtest/gtest.h>
#include <any>
#include <typeinfo>
#include <vector>

#include "expression.h"
#include "metadata.h"

using namespace std;
using namespace codein;

TEST(ExpressionTests, EqualityExpressionTest)
{
    ExpressionNode expr{
        .opCode = OpCode::Eq,
        .leafOrChildren = vector<ExpressionNode>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(1)},
        }
    };

    Metadata metadata{{"a", tiInt}};
    vector<any> data{1};

    auto r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_EQ(any_cast<bool>(r), true);

    metadata[0].typeIndex = tiUint;
    data[0] = 1u;
    get<0>(get<1>(expr.leafOrChildren)[1].leafOrChildren) = 1u;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_EQ(any_cast<bool>(r), true);

    metadata[0].typeIndex = tiString;
    data[0] = "test"s;
    get<0>(get<1>(expr.leafOrChildren)[1].leafOrChildren) = "test"s;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_EQ(any_cast<bool>(r), true);
}
