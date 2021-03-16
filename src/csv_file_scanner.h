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

// read a line, slice it into fields(indeicated by comma) and store the fields in vector<string>
std::vector<std::string> parseLine(const std::string& line);
// convert typeName into corresponding type. if there is no matching type, return typeid of void
std::type_index convertToTypeid(const std::string& typeName);
// read a line from metadata file and return metadata for fields in csv file
Metadata parseLineMetadata(const std::string& line);

/**
 * @brief CSV file scanner iterator. Scan the given CSV-formatted file.
 */
class CsvFileScanner : public Iterator {
public:
    template <typename T, typename... ArgTs>
    friend std::unique_ptr<Iterator> makeIterator(ArgTs&&...);

    void open() override
    {
        if (!dfs_.is_open()) {
            dfs_.open(dataFileName_);
        }
    }

    void reopen() override
    {
        open();
        readLines_ = 0;
        errorLines_= 0;
    }

    bool hasMore() const override
    {
        return !dfs_.eof();
    }

    // return a next valid line that passed the filter test.
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
/**
 * @brief Construct a new Csv File Scanner object to read lines in a csv file
 * 
 * @param metadataFileName: name of the file that contains metadata about csv file
 * @param dataFileName: name of the csv file that this CsvFileScanner will read 
 * @param expr: Expression to filter desired lines. if not specified, pass every line
 */
    CsvFileScanner(const std::string& metadataFileName,
        const std::string& dataFileName, const Expression& filterExpr = kAlwaysTrue);

    // helper function for processNext to check if there are too many error lines
    void checkError();

    // const threshold to decide if there are too many error lines
    const unsigned int kThreshold = 30;

    // metadata about the csv file
    Metadata metadata_;
    // name of the csv file name
    std::string dataFileName_;
    // file stream to open and read csv file and metadata file
    mutable std::fstream dfs_;
    // expression based on which this object will filter lines
    const Expression filterExpr_;
    // number of lines read by this object at a moment
    unsigned int readLines_;
    // number of error lines that were discrepant with metadata_
    unsigned int errorLines_;
};

// Invalid Metadata exception for when metadata file contains an non-existent type name
class InvalidMetadata {};
// exception for when CsvFileScanner cannot find a specified file
class NonExistentFile {};
// exception for when there are too many error lines and thus metadata can be considered invalid
// for csv file
class WrongMetadata {};

} // namespace codein