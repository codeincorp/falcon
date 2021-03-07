#include <gtest/gtest.h>
#include <any>
#include <typeinfo>
#include <vector>

#include "expression.h"
#include "metadata.h"

using namespace std;
using namespace codein;

TEST(ExpressionTests, EqualExpressionTest)
{
    // expression: a == 1
    Expression expr{
        .opCode = OpCode::Eq,
        .leafOrChildren = vector<Expression>{
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
    expr.second().leaf() = 1u;

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
    expr.second().leaf() = "test"s;

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
    expr.second().leaf() = 1.1;

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

TEST(ExpressionTests, NotEqualExpressionTest)
{
    // expression: a == 1
    Expression expr{
        .opCode = OpCode::Neq,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(1)},
        }
    };

    // data: a == 0
    Metadata metadata{{"a", tiInt}};
    vector<any> data{0};

    auto r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 1
    data[0] = 1;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));

    // expression: a == 1u
    expr.second().leaf() = 1u;

    // data: a == 2u
    metadata[0].typeIndex = tiUint;
    data[0] = 2u;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 1u
    data[0] = 1u;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));

    // expression: a == "test";
    expr.second().leaf() = "test"s;

    // data: a == "tess"
    metadata[0].typeIndex = tiString;
    data[0] = "tess"s;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == "test"
    data[0] = "test"s;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));

    // expression: a == 1.1
    expr.second().leaf() = 1.1;

    // data: a == 1.2
    metadata[0].typeIndex = tiDouble;
    data[0] = 1.2;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 1.1
    data[0] = 1.1;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));
}


TEST(ExpressionTests, LessExpressionTest)
{
    // expression: a < 3
    Expression expr{
        .opCode = OpCode::Lt,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(3)},
        }
    };

    // data: a == 1
    Metadata metadata{{"a", tiInt}};
    vector<any> data{1};

    auto r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 4
    data[0] = 4;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));

    // expression: a < 3u
    expr.second().leaf() = 3u;

    // data: a == 1u
    metadata[0].typeIndex = tiUint;
    data[0] = 1u;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 4u
    data[0] = 4u;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));

    // expression: a < "test"
    expr.second().leaf() = "test"s;

    // data: a == "tess"
    metadata[0].typeIndex = tiString;
    data[0] = "tess"s;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == "test"
    metadata[0].typeIndex = tiString;
    data[0] = "test"s;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));

    // expression: a < 2.2
    expr.second().leaf() = 2.2;

    // data: a == 1.1
    metadata[0].typeIndex = tiDouble;
    data[0] = 1.1;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 2.2
    metadata[0].typeIndex = tiDouble;
    data[0] = 2.2;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));
}

TEST(ExpressionTests, LessEqualExpressionTest)
{
    // expression: a <= 3
    Expression expr{
        .opCode = OpCode::Lte,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(3)},
        }
    };

    // data: a == 1
    Metadata metadata{{"a", tiInt}};
    vector<any> data{1};

    auto r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 3
    data[0] = 3;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 5
    data[0] = 5;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));

    // expression: a <= 3u
    expr.second().leaf() = 3u;

    // data: a == 1u
    metadata[0].typeIndex = tiUint;
    data[0] = 1u;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 3u
    data[0] = 3u;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 5u
    data[0] = 5u;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));

    // expression: a <= "test"
    expr.second().leaf() = "test"s;

    // data: a == "tess"
    metadata[0].typeIndex = tiString;
    data[0] = "tess"s;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == "test"
    data[0] = "test"s;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == "tesu"
    data[0] = "tesu"s;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));

    // expression: a <= 2.2
    expr.second().leaf() = 2.2f;

    // data: a == 1.1
    metadata[0].typeIndex = tiDouble;
    data[0] = 1.1f;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 2.2
    data[0] = 2.2f;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 3.3
    data[0] = 3.3f;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));
}

TEST(ExpressionTests, GreaterExpressionTest)
{
    // expression: a > 3
    Expression expr{
        .opCode = OpCode::Gt,
        .leafOrChildren = vector<Expression>{
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
    expr.second().leaf() = 3u;

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
    expr.second().leaf() = "test"s;

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
    expr.second().leaf() = 2.2;

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

TEST(ExpressionTests, GreaterEqualExpressionTest)
{
    // expression: a >= 3
    Expression expr{
        .opCode = OpCode::Gte,
        .leafOrChildren = vector<Expression>{
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

    // data: a == 3
    data[0] = 3;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 2
    data[0] = 2;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));

    // expression: a > 3u
    expr.second().leaf() = 3u;

    // data: a == 4u
    metadata[0].typeIndex = tiUint;
    data[0] = 4u;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 3u
    data[0] = 3u;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 1u
    data[0] = 1u;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));

    // expression: a > "test"
    expr.second().leaf() = "test"s;

    // data: a == "tesu"
    metadata[0].typeIndex = tiString;
    data[0] = "tesu"s;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

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

    // expression: a > 2.2
    expr.second().leaf() = 2.2;

    // data: a == 3.3
    metadata[0].typeIndex = tiDouble;
    data[0] = 3.3;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 2.2
    metadata[0].typeIndex = tiDouble;
    data[0] = 2.2;

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
    Expression expr{
        .opCode = OpCode::Add,
        .leafOrChildren = vector<Expression>{
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
    expr.first() = { .opCode = OpCode::Const, .leafOrChildren = std::any(5) };

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == tiInt);
    EXPECT_EQ(any_cast<int>(r), 7);
}
