#include <any>
#include <memory>
#include <optional>
#include <vector>

#include "iterator.h"
#include "expression.h"

#pragma once

namespace codein {

class InvalidFilter {};

class Filter : public Iterator {
public:
    template <typename T, typename... Args_>
    friend std::unique_ptr<Iterator> makeIterator(Args_&&...);

    void open() override
    {
        child_->open();
    }

    void reopen() override
    {
        child_->reopen();
    }

    bool hasMore() const override
    {
        return child_->hasMore();
    }

    std::optional<std::vector<std::any>> processNext() override;

    void close() override
    {
        child_->close();
    }

    const Metadata& getMetadata() const override
    {
        return metadata_;
    }

    ~Filter()
    {}

private:
    Filter(std::unique_ptr<Iterator>&& child, const Expression& expr)
        : child_()
        , expr_(expr)
        , metadata_()
    {
        if (expr_.opCode == OpCode::Noop) {
            throw InvalidFilter();
        }

        metadata_ = child->getMetadata();
        child_ = std::move(child);
    }

    std::unique_ptr<Iterator> child_;
    const Expression expr_;
    Metadata metadata_;
};

}
