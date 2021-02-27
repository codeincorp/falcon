#include <any>
#include <memory>
#include <optional>
#include <vector>

#include "metadata.h"

#pragma once

namespace codein {

struct Iterator {
    virtual void open() = 0;
    virtual void reopen() = 0;
    virtual bool hasMore() const = 0;
    virtual std::optional<std::vector<std::any>> processNext() = 0;
    virtual void close() = 0;
    virtual const Metadata& getMetadata() const = 0;
    virtual ~Iterator() {}
};

template <typename T, typename... Args_>
static std::unique_ptr<Iterator> make_iterator(Args_&&... args)
{
    return std::unique_ptr<Iterator>{new T(std::forward<Args_>(args)...)};
}

}
