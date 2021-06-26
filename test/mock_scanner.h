/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include <any>
#include <memory>
#include <optional>

#include "csv_file_scanner.h"
#include "iterator.h"

namespace codein {

class MockScanner : public codein::Iterator {
public:
    template <typename T, typename... ArgTs>
    friend std::unique_ptr<codein::Iterator> makeIterator(ArgTs&&...);

    void open() override
    {
        it_ = lines_.cbegin();
        isOpen_ = true;
    }

    void reopen() override
    {
        assert(isOpen_);

        open();
    }

    bool hasNext() const override
    {
        assert(isOpen_);

        return it_ != lines_.cend();
    }

    std::optional<std::vector<std::any>> processNext() override
    {
        assert(isOpen_);

        if (!hasNext()) {
            return std::nullopt;
        }

        auto fields = codein::parseLine(*it_);
        ++it_;

        assert(fields.size() == metadata_.size());

        std::vector<std::any> r;
        for (size_t i = 0; i < metadata_.size(); ++i) {
            r.emplace_back(codein::convertTo(metadata_[i].typeIndex, fields[i]));
        }

        return r;
    }

    void close() override
    {}

    const codein::Metadata& getMetadata() const override
    {
        return metadata_;
    }

    ~MockScanner() override
    {}

    MockScanner(const codein::Metadata& metadata, const std::vector<std::string>& lines)
        : metadata_(metadata)
        , lines_(lines)
        , it_(lines_.cend())
        , isOpen_(false)
    {}

private:
    codein::Metadata metadata_;
    std::vector<std::string> lines_;
    std::vector<std::string>::const_iterator it_;
    bool isOpen_;
};

}
