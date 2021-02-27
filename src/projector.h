#include <any>
#include <memory>
#include <optional>
#include <vector>
#include <utility>

#include "iterator.h"

#pragma once

namespace codein {

class Projector : public Iterator {
public:
    template <typename T, typename... Args_>
    friend std::unique_ptr<Iterator>
    make_iterator(Args_&...);

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
    Projector(const std::vector<std::string>& columns, std::unique_ptr<Iterator>&& child);

    std::unique_ptr<Iterator> child_;
    Metadata outputMetadata_;
    std::vector<size_t> colBinds_;
};

} // namespace codein