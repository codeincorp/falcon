#include <gtest/gtest.h>
#include <any>
#include <sstream>
#include <string>
#include <vector>

#include "any_visitor.h"

using namespace std;
using namespace codein;

TEST(AnyVisitorTests, SuccessfulCases)
{
    stringstream sstrm;
    vector<any> va{1, UINT32_MAX, 1.1f, 0.345, string("abc")};

    sstrm << va;
    string str = sstrm.str();

    auto pos = str.find("1", 0);
    EXPECT_NE(pos, string::npos) << "output = " << str;

    pos = str.find(to_string(UINT32_MAX), pos);
    EXPECT_NE(pos, string::npos) << "output = " << str;

    pos = str.find("1.1", pos);
    EXPECT_NE(pos, string::npos) << "output = " << str;

    pos = str.find("0.345", pos);
    EXPECT_NE(pos, string::npos) << "output = " << str;

    pos = str.find("abc", pos);
    EXPECT_NE(pos, string::npos) << "output = " << str;
}

TEST(AnyVisitorTests, FailureCases)
{
    stringstream sstrm;
    vector<any> va{1ull, LDBL_MAX, INT64_MIN};

    sstrm << va;
    string str = sstrm.str();

    auto pos = str.find("1", 0);
    EXPECT_EQ(pos, string::npos) << "output = " << str;

    pos = str.find(to_string(LDBL_MAX), 0);
    EXPECT_EQ(pos, string::npos) << "output = " << str;

    pos = str.find(to_string(INT64_MIN), 0);
    EXPECT_EQ(pos, string::npos) << "output = " << str;
}