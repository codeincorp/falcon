#include <any>
#include <memory>
#include <optional>
#include <vector>

#include "metadata.h"

#pragma once

namespace codein {

/**
 * @brief Interface for all iterators.
 * 
 * Iterator interface defines all methods that all iterators must implement.
 * open(), reopen(), hasMore(), processNext(), close(), and getMetadata().
 */
struct Iterator {
    /**
     * @brief Prepare the iterator so that it can process data.
     * 
     * Anything that needs to be done to process data should be implemented here
     * such as resource acquisition, preparing data, and etc.
     */
    virtual void open() = 0;

    virtual void reopen() = 0;

    /**
     * @brief Check whether there's more data to process.
     * 
     * @return true if there's more data to process.
     * @return false otherwise. 
     */
    virtual bool hasMore() const = 0;

    /**
     * @brief Process the next data and return the data
     * 
     * @return std::optional<std::vector<std::any>> 
     * If there's no more data, return value must be std::nullopt.
     * Otherwise, return value's has_value() must return true.
     */
    virtual std::optional<std::vector<std::any>> processNext() = 0;

    virtual void close() = 0;

    virtual const Metadata& getMetadata() const = 0;

    virtual ~Iterator() {}
};

/**
 * @brief The factory method that creates any type of iterator.
 * 
 * @tparam T 
 * @tparam Args_ 
 * @param args 
 * @return std::unique_ptr<Iterator> 
 */
template <typename T, typename... Args_>
static std::unique_ptr<Iterator> make_iterator(Args_&&... args)
{
    return std::unique_ptr<Iterator>{new T(std::forward<Args_>(args)...)};
}

}
