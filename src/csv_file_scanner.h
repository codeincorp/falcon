#include <any>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <fstream>

#include "metadata.h"
#include "iterator.h"
#include "expression.h"

#pragma once

namespace codein {

std::vector<std::string> parseLine(const std::string& line);
std::type_index convertToTypeid(const std::string& typeName);
Metadata parseLineMetadata(const std::string& line);

/**
 * @brief CSV file scanner iterator. Scan the given CSV-formatted file.
 */
class CsvFileScanner : public Iterator {
public:
    template <typename T, typename... Args_>
    friend std::unique_ptr<Iterator>
    makeIterator(Args_&&...);

    void open() override
    {
        if (!dfs_.is_open()) {
            dfs_.open(dataFileName_);
        }
    }

    void reopen() override
    {
        open();
    }

    bool hasMore() const override
    {
        return !dfs_.eof();
    }

    std::optional<std::vector<std::any>> processNext() override;

    void close() override
    {
        dfs_.close();
    }

    const Metadata& getMetadata() const override
    {
        return metadata_;
    }

    ~CsvFileScanner() override
    {
        if (dfs_.is_open()) {
            dfs_.close();
        }
    }

private:
    CsvFileScanner(const std::string& metadataFileName,
        const std::string& dataFileName, const Expression& expr = Expression{});

    Metadata metadata_;
    std::string dataFileName_;
    mutable std::fstream dfs_;
    const Expression expr_;
};

class InvalidMetadata {};
class NonExistentFile {};

} // namespace codein
