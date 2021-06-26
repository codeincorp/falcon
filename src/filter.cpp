/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include <any>
#include <memory>
#include <optional>

#include "filter.h"

namespace codein {

std::optional<std::vector<std::any>> Filter::processNext()
{
    std::optional<std::vector<std::any>> data;

    while (child_->hasNext()) {
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
