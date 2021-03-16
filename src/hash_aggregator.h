#include <any>
#include <memory>
#include <optional>
#include <vector>
#include <unordered_map>
#include <utility>

#include "iterator.h"
#include "expression.h"

#pragma once

namespace codein {

/**
 * @brief Hash aggregation iterator. Selects columns from input columns.
 */
class HashAggregator : public Iterator {
public:
    template <typename T, typename... ArgTs>
    friend std::unique_ptr<Iterator> makeIterator(ArgTs&&...);

    void open() override
    {}

    void reopen() override
    {}

    bool hasMore() const override
    {
        return false;
    }

    /**
     * @brief 
     * 
     * @return std::optional<std::vector<std::any>> 
     */
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
     * @brief Construct a new HashAggregator object
     * 
     * @param child 
     * @param outputMetadata Includes aggregation variables
     * @param groupKeyCols Columns for group key
     * @param aggExprs Aggregation expressions
     */
    HashAggregator(
        std::unique_ptr<Iterator>&& child,
        const Metadata& outputMetadata,
        const std::vector<std::string>& groupKeyCols,
        const std::vector<Expression>& aggExprs);

    Expression createHashExpr(const std::vector<std::string>& groupKeyCols);

    class HashGroupKey {

    private:
        const Metadata& metadata;
        const Expression& hashExpr_;
    };

    constexpr static auto hashGroupKeys = [&hashExpr_, &groupKeyMetadata_](const std::vector<std::any>& groupKeyVals) {
        return std::any_cast<std::size_t>(hashExpr_.eval(groupKeyMetadata_, groupKeyVals));
    };

    std::unique_ptr<Iterator> child_;
    const Metadata inputMetadata_;
    const Metadata outputMetadata_;
    const Metadata groupKeyMetadata_;
    const Expression hashExpr_;
    const std::vector<Expression> aggExprs_;
    // std::unordered_map<std::vector<std::any>
};

} // namespace codein
