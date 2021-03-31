#include "iterator.h"

#pragma once

namespace codein {

/**
 * @brief Limiter Iterator to limit number of output data from its child Iterator.
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

    std::optional<std::vector<std::any>> processNext();

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
     * @brief Construct a new Limiter object.
     * 
     * @param child: child Iterator.
     * @param limit: limit number of output data for child Iterator.
     */
    Limiter(std::unique_ptr<Iterator>&& child, size_t limit);

    // child Iterator that will output data.
    std::unique_ptr<Iterator> child_;
    // limit of output data on the child Iterator.
    size_t limit_;
    // current number of already output data.
    size_t curOutput_;
};


} // namespace codein
