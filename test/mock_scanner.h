#include <any>
#include <memory>
#include <optional>

#include "iterator.h"
#include "csv_file_scanner.h"

class MockScanner : public codein::Iterator {
public:
    template <typename T, typename... ArgTs>
    friend std::unique_ptr<codein::Iterator> makeIterator(ArgTs&&...);

    void open() override
    {
        it_ = lines_.cbegin();
    }

    void reopen() override
    {
        open();
    }

    bool hasMore() const override
    {
        return it_ != lines_.cend();
    }

    std::optional<std::vector<std::any>> processNext() override
    {
        if (!hasMore()) {
            return std::nullopt;
        }

        auto fields = codein::parseLine(*it_);
        ++it_;

        assert(fields.size() == metadata_.size());

        std::vector<std::any> r;
        for (size_t i = 0; i < metadata_.size(); ++i) {
            r.emplace_back(codein::convertTo(codein::anyConverters, metadata_[i].typeIndex, fields[i]));
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
    {}

private:
    codein::Metadata metadata_;
    std::vector<std::string> lines_;
    std::vector<std::string>::const_iterator it_;
};

