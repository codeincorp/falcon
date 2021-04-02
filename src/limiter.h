#include "iterator.h"

#pragma once

namespace codein {

/**
 * @brief Limiter iterator limits number of output data from its child iterator.
 */
class Limiter : public Iterator {
public:
    template <typename T, typename... ArgTs>
    friend std::unique_ptr<Iterator> makeIterator(ArgTs&&...);

    void open()
    {
        child_->open();
        curOutput_ = 0;
    }

    void reopen() override 
    {
        open();
    }

    bool hasMore() const override 
    {
        return curOutput_ < limit_ && child_->hasMore();
    }

    std::optional<std::vector<std::any>> processNext() override;

    void close() override
    {
        child_->close();
    }

    const Metadata& getMetadata() const override
    {
        return child_->getMetadata();
    }

    ~Limiter() override {}
    
private: 
    /**
     * @brief Constructs a new Limiter object.
     * 
     * @param child: child iterator.
     * @param limit: limit number of output data from child iterator.
     * If child iterator can output less data than specified limit, 
     * Limiter iterator will stop outputing data before reaching the limit.
     */
    Limiter(std::unique_ptr<Iterator>&& child, size_t limit)
        : child_(std::move(child))
        , limit_(limit)
        , curOutput_(limit)
    {}

    std::unique_ptr<Iterator> child_;

    const size_t limit_;
    // current number of already output data.
    size_t curOutput_;
};

} // namespace codein
