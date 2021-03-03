#include <any>
#include <memory>
#include <optional>
#include <string>
#include <vector>

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
        it_ = lines_.cbegin();
    }

    void reopen() override {};

    bool hasMore() const override
    {
        return it_ != lines_.cend();
    }

    std::optional<std::vector<std::any>> processNext() override;

    void close() override
    {
        it_ = lines_.cend();
    }

    const Metadata& getMetadata() const override
    {
        return metadata_;
    }

    ~CsvFileScanner() override
    {
        it_ = lines_.cend();
    }

private:
    /**
     * @brief Construct a new CSV File Scanner object.
     * Should not be used directly. Instead, make_iterator() should be used.
     * 
     * @param metadata
     * @param lines
     */
    CsvFileScanner(const Metadata& metadata, const std::vector<std::string>& lines)
        : metadata_(metadata)
        , lines_(lines)
        , it_(lines_.cend())
    {};

    CsvFileScanner(const std::string& metadataFileName,
        const std::string& dataFileName, const ExpressionNode& expr = ExpressionNode{});

    Metadata metadata_;
    std::vector<std::string> lines_;
    std::vector<std::string>::const_iterator it_;
    const ExpressionNode expr_;
};

class InvalidMetadata {};
class NonExistentFile {};

} // namespace codein
