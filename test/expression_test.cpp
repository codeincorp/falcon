#include <gtest/gtest.h>
#include <any>
#include <typeinfo>
#include <variant>
#include <vector>

#include "expression.h"
#include "metadata.h"
#include "any_visitor.h"

using namespace std;
using namespace codein;

TEST(ExpressionTests, BasicTests)
{
    // leaf-only expression: const(1)
    Expression expr{
        .opCode = OpCode::Const,
        .leafOrChildren = std::any(1)
    };

    // data: a == 123
    Metadata metadata{{"a", tiInt}};
    vector<any> data{123};

    auto r = expr.eval(metadata, data);
    EXPECT_EQ(any_cast<int>(r), 1);

    // expression: ref(a)
    expr.opCode = OpCode::Ref;
    expr.leaf() = std::any("a"s);

    r = expr.eval(metadata, data);
    EXPECT_EQ(any_cast<int>(r), 123);

    // A simple tree expression: !false -> Not --- Const(false)
    expr.opCode = OpCode::Not;
    expr.leafOrChildren = vector<Expression>{
        {.opCode = OpCode::Const, .leafOrChildren = std::any(false)},
    };

    r = expr.eval(metadata, data);
    EXPECT_TRUE(any_cast<bool>(r));

    // More complex tree expression: a > 1 && a <= 5
    // And -+- Gt  -+- Ref(a)
    //      |       |
    //      |       +- Const(1)
    //      | 
    //      +- Lte -+- Ref(a)
    //              |
    //              +- Const(5)
    expr.opCode = OpCode::And;
    expr.leafOrChildren = vector<Expression>{
        // a > 1
        {
            .opCode = OpCode::Gt,
            .leafOrChildren = vector<Expression>{
                {
                    .opCode = OpCode::Ref,
                    .leafOrChildren = std::any("a"s)
                },
                {
                    .opCode = OpCode::Const,
                    .leafOrChildren = std::any(1)
                }
            }
        },
        // a <= 5
        {
            .opCode = OpCode::Lte,
            .leafOrChildren = vector<Expression>{
                {
                    .opCode = OpCode::Ref,
                    .leafOrChildren = std::any("a"s)
                },
                {
                    .opCode = OpCode::Const,
                    .leafOrChildren = std::any(5)
                }
            }
        },
    };

    // data: a = 3
    data[0] = 3;

    r = expr.eval(metadata, data);
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a = -10
    data[0] = -10;

    r = expr.eval(metadata, data);
    EXPECT_FALSE(any_cast<bool>(r));

    // data: a = 8
    data[0] = 8;

    r = expr.eval(metadata, data);
    EXPECT_FALSE(any_cast<bool>(r));
}

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

    // data: bool
    metadata[0].typeIndex = tiBool;
    expr = {
        .opCode = OpCode::Lt,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(false)},
        }
    };

    data[0] = false;
    r = expr.eval(metadata, data);
    EXPECT_FALSE(any_cast<bool>(r)); 

    data[0] = true;
    r = expr.eval(metadata, data);
    EXPECT_FALSE(any_cast<bool>(r)); 

    expr = {
        .opCode = OpCode::Lt,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(true)},
        }
    };

    data[0] = false;
    r = expr.eval(metadata, data);
    EXPECT_TRUE(any_cast<bool>(r)); 

    data[0] = true;
    r = expr.eval(metadata, data);
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
    expr.second().leaf() = 2.2;

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
    EXPECT_TRUE(any_cast<bool>(r));

    // data: a == 3.3
    data[0] = 3.3;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == typeid(bool));
    EXPECT_FALSE(any_cast<bool>(r));

    //data: a = bool
    metadata[0].typeIndex = tiBool;
    expr = {
        .opCode = OpCode::Lte,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(false)},
        }
    };

    data[0] = false;
    r = expr.eval(metadata, data);
    EXPECT_TRUE(any_cast<bool>(r)); 

    data[0] = true;
    r = expr.eval(metadata, data);
    EXPECT_FALSE(any_cast<bool>(r)); 

    expr = {
        .opCode = OpCode::Lte,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(true)},
        }
    };

    data[0] = false;
    r = expr.eval(metadata, data);
    EXPECT_TRUE(any_cast<bool>(r)); 

    data[0] = true;
    r = expr.eval(metadata, data);
    EXPECT_TRUE(any_cast<bool>(r));  
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

    // true > false
    expr = {
        .opCode = OpCode::Gt,
        .leafOrChildren = vector<Expression> {
            {.opCode = OpCode::Const, .leafOrChildren = any(true)},
            {.opCode = OpCode::Const, .leafOrChildren = any(false)}
        }
    };

    r = expr.eval(metadata, data);
    EXPECT_TRUE(any_cast<bool>(r));

    // a > true
    expr = {
        .opCode = OpCode::Gt,
        .leafOrChildren = vector<Expression> {
            {.opCode = OpCode::Ref, .leafOrChildren = any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = any(true)}
        }
    };
    metadata = {{"a", tiBool}};
    data[0] = true;


    r = expr.eval(metadata, data);
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

    // data: a == true
    expr = {
        .opCode = OpCode::Gte,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(false)},
        }
    };

    metadata[0].typeIndex = tiBool;
    data[0] = true;

    r = expr.eval(metadata, data);

    EXPECT_TRUE(any_cast<bool>(r)); 

    expr = {
        .opCode = OpCode::Gte,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(true)},
        }
    };

    r = expr.eval(metadata, data);

    EXPECT_TRUE(any_cast<bool>(r)); 
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

TEST(ExpressionTests, SubExpressionTest)
{
    // expression: a - b
    Expression expr{
        .opCode = OpCode::Sub,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("b"s)},
        }
    };

    // data: a == 1, b == 2
    Metadata metadata{{"a", tiInt}, {"b", tiInt}};
    vector<any> data{1, 2};

    auto r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == tiInt);
    EXPECT_EQ(any_cast<int>(r), -1);

    // expression: 5 - b
    expr.first() = { .opCode = OpCode::Const, .leafOrChildren = std::any(5) };

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == tiInt);
    EXPECT_EQ(any_cast<int>(r), 3);
}

TEST(ExpressionTests, MultExpressionTest)
{
    // expression: a * b
    Expression expr{
        .opCode = OpCode::Mult,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("b"s)},
        }
    };

    // data: a == 2, b == 3
    Metadata metadata{{"a", tiInt}, {"b", tiInt}};
    vector<any> data{2, 3};

    auto r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == tiInt);
    EXPECT_EQ(any_cast<int>(r), 6);

    // expression: 5 * b
    expr.first() = { .opCode = OpCode::Const, .leafOrChildren = std::any(5) };

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == tiInt);
    EXPECT_EQ(any_cast<int>(r), 15);
}

TEST(ExpressionTests, DivExpressionTest)
{
    // expression: a / b
    Expression expr{
        .opCode = OpCode::Div,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("b"s)},
        }
    };

    // data: a == 6, b == 3
    Metadata metadata{{"a", tiInt}, {"b", tiInt}};
    vector<any> data{6, 3};

    auto r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == tiInt);
    EXPECT_EQ(any_cast<int>(r), 2);

    // expression: 1 / b
    expr.first() = { .opCode = OpCode::Const, .leafOrChildren = std::any(1) };

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == tiInt);
    EXPECT_EQ(any_cast<int>(r), 0);
}

TEST(ExpressionTests, ModExpressionTest)
{
    // expression: a % b
    Expression expr{
        .opCode = OpCode::Mod,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("b"s)},
        }
    };

    // data: a == 5, b == 3
    Metadata metadata{{"a", tiInt}, {"b", tiInt}};
    vector<any> data{5, 3};

    auto r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == tiInt);
    EXPECT_EQ(any_cast<int>(r), 2);

    // expression: 1 % b
    expr.first() = { .opCode = OpCode::Const, .leafOrChildren = std::any(1) };

    r = expr.eval(metadata, data);

    EXPECT_TRUE(r.type() == tiInt);
    EXPECT_EQ(any_cast<int>(r), 1);
}

TEST(ExpressionTests, UnsupportedOperationTest)
{
    // a(string) == b(string) - "1"
    Expression expr {
        .opCode = OpCode::Eq,
        .leafOrChildren = vector<Expression>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {
                .opCode = OpCode::Sub, 
                .leafOrChildren = vector<Expression>{
                    {.opCode = OpCode::Ref, .leafOrChildren = std::any("b"s)},
                    {.opCode = OpCode::Const, .leafOrChildren = std::any("1"s)},
                }
            },
            
        }
    };

    Metadata metadata { {"a", tiString}, {"b", tiString} };
    vector<any> data {"OTTOGI"s, "OTTOGI"s };

    EXPECT_THROW(expr.eval(metadata, data), UnsupportedOperation);

    EXPECT_THROW(notAny(any(1)), UnsupportedOperation);

    // UINT64 != UINT64
    expr = {
        .opCode = OpCode::Neq,
        .leafOrChildren = vector<Expression> {
            {.opCode = OpCode::Const, .leafOrChildren = (uint64_t)UINT64_MAX},
            {.opCode = OpCode::Const, .leafOrChildren = (uint64_t)(UINT64_MAX-1)}
        }
    };
    EXPECT_THROW(expr.eval(metadata, data), UnsupportedOperation);
    // UINT64 < UINT64
    expr.opCode = OpCode::Lt;
    EXPECT_THROW(expr.eval(metadata, data), UnsupportedOperation);
    // UINT64 <= UINT64
    expr.opCode = OpCode::Lte;
    EXPECT_THROW(expr.eval(metadata, data), UnsupportedOperation);

    // UINT64 > UINT64
    expr.opCode = OpCode::Gt;
    EXPECT_THROW(expr.eval(metadata,data), UnsupportedOperation);

    // UINT64 >= UINT64
    expr.opCode = OpCode::Gte;
    EXPECT_THROW(expr.eval(metadata,data), UnsupportedOperation);

    // 4.5 % 1.2
    expr = {
        .opCode = OpCode::Mod,
        .leafOrChildren = vector<Expression> {
            {.opCode = OpCode::Const, .leafOrChildren = any(4.5)},
            {.opCode = OpCode::Const, .leafOrChildren = any(1.2)}
        }
    };
    EXPECT_THROW(expr.eval(metadata, data), UnsupportedOperation);

    // "45" / "15"
    expr = {
        .opCode = OpCode::Div,
        .leafOrChildren = vector<Expression> {
            {.opCode = OpCode::Const, .leafOrChildren = any("45"s)},
            {.opCode = OpCode::Const, .leafOrChildren = any("15"s)}
        }
    };
    EXPECT_THROW(expr.eval(metadata,data), UnsupportedOperation);

    expr.opCode = OpCode::Mult;
    EXPECT_THROW(expr.eval(metadata,data), UnsupportedOperation);

    expr.opCode = OpCode::Sub;
    EXPECT_THROW(expr.eval(metadata,data), UnsupportedOperation);

    expr.opCode = OpCode::Mod;
    EXPECT_THROW(expr.eval(metadata,data), UnsupportedOperation);

    // 4 + 3.4f
    expr = {
        .opCode = OpCode::Add,
        .leafOrChildren = vector<Expression> {
            {.opCode = OpCode::Const, .leafOrChildren = any(4)},
            {.opCode = OpCode::Const, .leafOrChildren = any(3.4f)}
        }
    };
    EXPECT_THROW(expr.eval(metadata,data), UnsupportedOperation);

    // true + false
    expr = {
        .opCode = OpCode::Add,
        .leafOrChildren = vector<Expression> {
            {.opCode = OpCode::Const, .leafOrChildren = any(true)},
            {.opCode = OpCode::Const, .leafOrChildren = any(false)}
        }
    }; 
    EXPECT_THROW(expr.eval(metadata,data), UnsupportedOperation);
}
