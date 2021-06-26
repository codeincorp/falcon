/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include <any>
#include <memory>
#include <optional>
#include <vector>

#include "expression.h"
#include "iterator.h"

#pragma once

namespace codein {

class InvalidFilter {};

class Filter : public Iterator {
public:
    template <typename T, typename... ArgTs>
    friend std::unique_ptr<Iterator> makeIterator(ArgTs&&...);

    void open() override
    {
        child_->open();
    }

    void reopen() override
    {
        child_->reopen();
    }

    bool hasNext() const override
    {
        return child_->hasNext();
    }

    std::optional<std::vector<std::any>> processNext() override;

    void close() override
    {
        child_->close();
    }

    const Metadata& getMetadata() const override
    {
        return metadata_;
    }

    ~Filter()
    {}

private:
    Filter(std::unique_ptr<Iterator>&& child, const Expression& expr)
        : child_()
        , expr_(expr)
        , metadata_()
    {
        if (expr_.opCode == OpCode::Noop) {
            throw InvalidFilter();
        }

        metadata_ = child->getMetadata();
        child_ = std::move(child);
    }

    std::unique_ptr<Iterator> child_;
    const Expression expr_;
    Metadata metadata_;
};

}
