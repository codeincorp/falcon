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
    // expression: a == 1
    ExpressionNode expr{
        .opCode = OpCode::Eq,
        .leafOrChildren = vector<ExpressionNode>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(1)},
        }
    };

    // data: a == 1
    Metadata metadata{{"a", tiInt}};
    vector<any> data{1};

    auto r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 2
    data[0] = 2;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));

    // expression: a == 1u
    get<0>(get<1>(expr.leafOrChildren)[1].leafOrChildren) = 1u;
    // data: a == 1u
    metadata[0].typeIndex = tiUint;
    data[0] = 1u;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 2u
    data[0] = 2u;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));

    // expression: a == "test";
    get<0>(get<1>(expr.leafOrChildren)[1].leafOrChildren) = "test"s;
    // data: a == "test"
    metadata[0].typeIndex = tiString;
    data[0] = "test"s;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == "tess"
    data[0] = "tess"s;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));

    // expression: a == 1.1
    get<0>(get<1>(expr.leafOrChildren)[1].leafOrChildren) = 1.1;
    // data: a == 1.1
    metadata[0].typeIndex = tiDouble;
    data[0] = 1.1;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 2.2
    data[0] = 2.2;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));
}

TEST(ExpressionTests, LessThanExpressionTest)
{
    // expression: a < 3
    ExpressionNode expr{
        .opCode = OpCode::Lt,
        .leafOrChildren = vector<ExpressionNode>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(3)},
        }
    };

    // a == 1
    Metadata metadata{{"a", tiInt}};
    vector<any> data{1};

    auto r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // expression: a < 3u
    get<0>(get<1>(expr.leafOrChildren)[1].leafOrChildren) = 3u;
    // a == 1u
    metadata[0].typeIndex = tiUint;
    data[0] = 1u;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // expression: a < "test"
    get<0>(get<1>(expr.leafOrChildren)[1].leafOrChildren) = "test"s;
    // a == "tess"
    metadata[0].typeIndex = tiString;
    data[0] = "tess"s;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // expression: a < 2.2
    get<0>(get<1>(expr.leafOrChildren)[1].leafOrChildren) = 2.2;
    // a == 1.1
    metadata[0].typeIndex = tiDouble;
    data[0] = 1.1;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));
}

TEST(ExpressionTests, GreaterThanExpressionTest)
{
    // expression: a > 3
    ExpressionNode expr{
        .opCode = OpCode::Gt,
        .leafOrChildren = vector<ExpressionNode>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(3)},
        }
    };

    // data: a == 4
    Metadata metadata{{"a", tiInt}};
    vector<any> data{4};

    auto r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 2
    data[0] = 2;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));

    // expression: a > 3u
    get<0>(get<1>(expr.leafOrChildren)[1].leafOrChildren) = 3u;
    // data: a == 4u
    metadata[0].typeIndex = tiUint;
    data[0] = 4u;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 1u
    data[0] = 1u;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));

    // expression: a > "test"
    get<0>(get<1>(expr.leafOrChildren)[1].leafOrChildren) = "test"s;
    // data: a == "tesu"
    metadata[0].typeIndex = tiString;
    data[0] = "tesu"s;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == "tess"
    data[0] = "tess"s;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));

    // expression: a > 2.2
    get<0>(get<1>(expr.leafOrChildren)[1].leafOrChildren) = 2.2;
    // data: a == 3.3
    metadata[0].typeIndex = tiDouble;
    data[0] = 3.3;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 1.1
    data[0] = 1.1;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));
}

TEST(ExpressionTests, AddExpressionTest)
{
    // expression: a + b
    ExpressionNode expr{
        .opCode = OpCode::Add,
        .leafOrChildren = vector<ExpressionNode>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("b"s)},
        }
    };

    Metadata metadata{{"a", tiInt}, {"b", tiInt}};
    vector<any> data{1, 2};

    auto r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == tiInt);
    EXPECT_EQ(any_cast<int>(r), 3);

    // expression: 5 + b
    get<1>(expr.leafOrChildren)[0].opCode = OpCode::Const;
    get<1>(expr.leafOrChildren)[0].leafOrChildren = std::any(5);

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == tiInt);
    EXPECT_EQ(any_cast<int>(r), 7);
}
