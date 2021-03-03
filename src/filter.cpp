#include <any>
#include <memory>
#include <optional>

#include "filter.h"

namespace codein {

std::optional<std::vector<std::any>> Filter::processNext()
{
    std::optional<std::vector<std::any>> data;

    while (child_->hasMore()) {
        data = child_->processNext();
        if (!data.has_value()) {
            break;
        }

        if (auto r = expr_.eval(metadata_, data.value()); std::any_cast<bool>(r)) {
            return data;
        }
    }

    return std::nullopt;
}

}
