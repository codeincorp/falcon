#include <any>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "metadata.h"
#include "iterator.h"

#pragma once

namespace codein {

/**
 * @brief CSV file scanner
 */
class CsvFileScanner : public Iterator {
public:
    template <typename T, typename... Args_>
    friend std::unique_ptr<Iterator>
    make_iterator(Args_&&...);

    void open() override
    {
        it_ = lines_.cbegin();
    };

    void reopen() override {};

    bool hasMore() const override
    {
        return it_ != lines_.cend();
    }

    std::optional<std::vector<std::any>> processNext() override;

    void close() override
    {
        it_ = lines_.cend();
    };

    const Metadata& getMetadata() const override
    {
        return metadata_;
    };

    ~CsvFileScanner() override
    {
        it_ = lines_.cend();
    };

private:
    CsvFileScanner(const Metadata& metadata, const std::vector<std::string>& lines)
        : metadata_(metadata)
        , lines_(lines)
        , it_(lines_.cend())
    {};

    Metadata metadata_;
    std::vector<std::string> lines_;
    std::vector<std::string>::const_iterator it_;
};

} // namespace codein
