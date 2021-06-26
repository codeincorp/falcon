/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include <vector>
#include <optional>
#include <any>

#include "limiter.h"

namespace codein {

std::optional<std::vector<std::any>> Limiter::processNext()
{
    if (!hasNext()) {
        return std::nullopt;
    }

    ++curOutput_;
    return child_->processNext();
}

} // namespace codein
