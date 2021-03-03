#include <gtest/gtest.h>
#include <memory>

#include "iterator.h"
#include "filter.h"
#include "csv_file_scanner.h"

using namespace std;
using namespace codein;

bool operator==(const Metadata& lhs, const Metadata& rhs);

class MockScanner : public Iterator {
public:
    template <typename T>
    friend std::unique_ptr<Iterator> makeIterator();

    void open() override
    {
        it_ = lines_.cbegin();
    }

    void reopen() override
    {
        open();
    }

    bool hasMore() const override
    {
        return it_ != lines_.cend();
    }

    optional<vector<any>> processNext() override
    {
        if (!hasMore()) {
            return nullopt;
        }

        auto fields = parseLine(*it_);
        ++it_;

        assert(fields.size() == metadata_.size());

        std::vector<std::any> r;
        for (size_t i = 0; i < metadata_.size(); ++i) {
            r.emplace_back(convertTo(anyConverters, metadata_[i].typeIndex, fields[i]));
        }

        return r;
    }

    void close() override
    {}

    const Metadata& getMetadata() const override
    {
        return metadata_;
    }

    ~MockScanner() override
    {}

    MockScanner()
    {}

private:
    Metadata metadata_{
        { "a", tiInt },
        { "b", tiFloat },
        { "c", tiString },
    };
    vector<string> lines_{
        "1,1.1,John Smith",
        "2,2.2,Alex Smith",
        "3,3.3,Alex Swanson",
    };
    vector<string>::const_iterator it_;
};

TEST(FilterTests, BasicTest)
{
    ExpressionNode filterExpr{
        .opCode = OpCode::Eq,
        .leafOrChildren = vector<ExpressionNode>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(2)},
        }
    };

    auto mockScanner = makeIterator<MockScanner>();

    auto filter = makeIterator<Filter>(move(mockScanner), filterExpr);

    filter->open();
    EXPECT_TRUE(filter->hasMore());

    auto data = filter->processNext();
    EXPECT_TRUE(data.has_value());

    const auto& val = data.value();
    EXPECT_EQ(any_cast<int>(val[0]), 2);
    EXPECT_EQ(any_cast<float>(val[1]), 2.2f);
    EXPECT_EQ(any_cast<string>(val[2]), "Alex Smith"s);

    EXPECT_TRUE(filter->hasMore());

    data = filter->processNext();
    EXPECT_FALSE(data.has_value());
}

TEST(FilterTests, PassThruTest)
{
    ExpressionNode filterExpr{
        .opCode = OpCode::Const,
        .leafOrChildren = std::any(true)
    };

    auto mockScanner = makeIterator<MockScanner>();

    auto filter = makeIterator<Filter>(move(mockScanner), filterExpr);

    filter->open();
    EXPECT_TRUE(filter->hasMore());

    auto data = filter->processNext();
    EXPECT_TRUE(data.has_value());

    const auto& val = data.value();
    EXPECT_EQ(any_cast<int>(val[0]), 1);
    EXPECT_EQ(any_cast<float>(val[1]), 1.1f);
    EXPECT_EQ(any_cast<string>(val[2]), "John Smith"s);

    EXPECT_TRUE(filter->hasMore());
    data = filter->processNext();
    EXPECT_TRUE(data.has_value());

    const auto& val2 = data.value();
    EXPECT_EQ(any_cast<int>(val2[0]), 2);
    EXPECT_EQ(any_cast<float>(val2[1]), 2.2f);
    EXPECT_EQ(any_cast<string>(val2[2]), "Alex Smith"s);

    EXPECT_TRUE(filter->hasMore());
    data = filter->processNext();

    const auto& val3 = data.value();
    EXPECT_EQ(any_cast<int>(val3[0]), 3);
    EXPECT_EQ(any_cast<float>(val3[1]), 3.3f);
    EXPECT_EQ(any_cast<string>(val3[2]), "Alex Swanson"s);
}

TEST(FilterTests, NoDataTest)
{
    ExpressionNode filterExpr{
        .opCode = OpCode::Eq,
        .leafOrChildren = vector<ExpressionNode>{
            {.opCode = OpCode::Ref, .leafOrChildren = std::any("a"s)},
            {.opCode = OpCode::Const, .leafOrChildren = std::any(0)},
        }
    };

    auto mockScanner = makeIterator<MockScanner>();

    auto filter = makeIterator<Filter>(move(mockScanner), filterExpr);

    filter->open();
    EXPECT_TRUE(filter->hasMore());

    auto data = filter->processNext();
    EXPECT_FALSE(data.has_value());
    EXPECT_FALSE(filter->hasMore());
}

TEST(FilterTests, InvalidFilterTest)
{
    ExpressionNode filterExpr{
        .opCode = OpCode::Noop,
        .leafOrChildren = std::any()
    };

    auto mockScanner = makeIterator<MockScanner>();

    EXPECT_THROW(makeIterator<Filter>(move(mockScanner), filterExpr), InvalidFilter);
}
