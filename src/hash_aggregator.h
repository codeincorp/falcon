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
     * @brief Constructs a new HashAggregator object.
     * 
     * @param child 
     * @param outputMetadata Includes aggregation variables.
     * @param groupKeyCols Columns for group key.
     * @param aggExprs Aggregation expressions.
     */
    HashAggregator(
        std::unique_ptr<Iterator>&& child,
        const Metadata& outputMetadata,
        const std::vector<std::string>& groupKeyCols,
        const std::vector<Expression>& aggExprs);

    struct Hasher {
        std::size_t operator()(const std::vector<std::any>& groupKeyVals) const
        {
            std::size_t h = 0;
            for (const auto& keyVal: groupKeyVals) {
                h ^= hashAny(keyVal);
            }

            return h;
        }
    };

    struct KeyEqual {
        bool operator()(const std::vector<std::any>& lhs, const std::vector<std::any>& rhs) const
        {
            std::size_t n = lhs.size();
            if (n != rhs.size()) {
                return false;
            }

            for (std::size_t i = 0; i < n; ++i) {
                if (lhs[i] != rhs[i]) {
                    return false;
                }
            }

            return true;
        }
    };

    using GroupKeyType = std::vector<std::any>;
    using HashStorageType = std::unordered_map<GroupKeyType, int, Hasher, KeyEqual>;

    static std::vector<Expression> createGroupKeyProjExprs(const Metadata&, const std::vector<std::string>&);

    std::unique_ptr<Iterator> child_;
    const Metadata inputMetadata_;
    const Metadata outputMetadata_;
    const std::vector<Expression> groupKeyProjExprs_;
    const std::vector<Expression> aggExprs_;
    HashStorageType hashStorage_;
    HashStorageType::const_iterator it_;
};

} // namespace codein
