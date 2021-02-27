#include <any>
#include <memory>
#include <optional>
#include <vector>
#include <utility>

#include "iterator.h"

#pragma once

namespace codein {

/**
 * @brief Projection iterator. Selects columns from input columns.
 */
class Projector : public Iterator {
public:
    template <typename T, typename... Args_>
    friend std::unique_ptr<Iterator>
    makeIterator(Args_&&...);

    /// Just open the child.
    void open() override
    {
        child_->open();
    }

    void reopen() override
    {
        child_->reopen();
    };

    bool hasMore() const override
    {
        return child_->hasMore();
    }

    /**
     * @brief 
     * 
     * @return std::optional<std::vector<std::any>> 
     */
    std::optional<std::vector<std::any>> processNext() override;

    void close() override
    {
        child_->close();
    }

    const Metadata& getMetadata() const override
    {
        return outputMetadata_;
    }

    ~Projector() override
    {}

private:
    /**
     * @brief Construct a new Projector object
     * 
     * @param columns 
     * @param child 
     */
    Projector(const std::vector<std::string>& columns, std::unique_ptr<Iterator>&& child);

    std::unique_ptr<Iterator> child_;
    Metadata outputMetadata_;
    std::vector<size_t> colBinds_;
};

} // namespace codein
