#include <any>
#include <memory>
#include <optional>
#include <tuple>
#include <vector>
#include <unordered_map>
#include <utility>

#include "iterator.h"
#include "any_visitor.h"
#include "expression.h"

#pragma once

namespace codein {

/**
 * @brief Expression for aggregation. Composed of an initialization expression
 * and continuation expression.
 */
struct AggregationExpression {
    Expression initExpr;
    Expression contExpr;
};

/**
 * @brief Hash aggregation iterator.
 */
class HashAggregator : public Iterator {
public:
    template <typename T, typename... ArgTs>
    friend std::unique_ptr<Iterator> makeIterator(ArgTs&&...);

    void open() override;

    void reopen() override
    {
        open();
    }

    bool hasMore() const override
    {
        return it_ != hashStorage_.cend();
    }

    std::optional<std::vector<std::any>> processNext() override;

    void close() override
    {}

    const Metadata& getMetadata() const override
    {
        return outputMetadata_;
    }

    ~HashAggregator() override
    {}

private:
    /**
     * @brief Constructs a new HashAggregator object.
     * 
     * @param child A child iterator.
     * @param groupKeyCols Columns for group key.
     * @param groupValMetadata Defines aggregation variables.
     * @param aggExprs Aggregation expressions.
     */
    HashAggregator(
        std::unique_ptr<Iterator>&& child,
        const std::vector<std::string>& groupKeyCols,
        const Metadata& groupValMetadata,
        const std::vector<AggregationExpression>& aggExprs);

    HashAggregator(
        std::unique_ptr<Iterator>&& child,
        const std::vector<std::string>& groupKeyCols,
        const Metadata& groupValMetadata,
        const Metadata& outputMetadata,
        const std::vector<AggregationExpression>& aggExprs,
        const std::vector<Expression>& outputProjs);

    struct Hasher {
        std::size_t operator()(const std::vector<std::any>& groupKeyVals) const;
    };

    struct KeyEqual {
        bool operator()(const std::vector<std::any>& lhs, const std::vector<std::any>& rhs) const;
    };

    using GroupKeyType = std::vector<std::any>;
    using GroupValueType = std::vector<std::any>;
    using HashStorageType = std::unordered_map<GroupKeyType, GroupValueType, Hasher, KeyEqual>;

    static std::vector<Expression> createGroupKeyProjExprs(const Metadata&, const std::vector<std::string>&);

    std::unique_ptr<Iterator> child_;
    Metadata inputMetadata_;
    Metadata groupMetadata_;
    Metadata outputMetadata_;
    const std::vector<Expression> groupKeyProjs_;
    const std::vector<AggregationExpression> aggExprs_;
    std::vector<Expression> outputProjs_;
    HashStorageType hashStorage_;
    HashStorageType::const_iterator it_;
};

} // namespace codein
