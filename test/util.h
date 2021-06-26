/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include <any>
#include <memory>
#include <vector>

#include "iterator.h"

#pragma once

void verifyIteratorOutput(
    const std::vector<std::vector<std::any>>& expectedFields, const std::unique_ptr<codein::Iterator>& iterator);
