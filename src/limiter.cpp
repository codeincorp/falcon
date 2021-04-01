#include <vector>
#include <optional>
#include <any>

#include "limiter.h"

namespace codein {

std::optional<std::vector<std::any>> Limiter::processNext()
{
    if (!hasMore()) {
        return std::nullopt;
    }

    ++curOutput_;
    return child_->processNext();
}

} // namespace codein
