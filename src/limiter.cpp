#include <vector>
#include <optional>
#include <any>

#include "limiter.h"

namespace codein {

Limiter::Limiter(std::unique_ptr<Iterator>&& child, size_t limit)
    : child_(std::move(child))
    , limit_(limit)
    , curOutput_(limit)
{}

std::optional<std::vector<std::any>> Limiter::processNext()
{
    if (!hasMore()) {
        return std::nullopt;
    }

    ++curOutput_;
    return child_->processNext();
}

} // namespace codein
