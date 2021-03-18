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

/** * @brief separate fields in a line, trimming extra spaces on both sides of each field. * * @param line 
 * @return vector<string> containing each field. */
std::vector<std::string> parseLine(const std::string& line);

/** * @brief converts string typeName into corresponding type_index. * * @param typeName
 * @return type_index of typeName if the type is supported. otherwise, return type_index of void. */
std::type_index convertToTypeid(const std::string& typeName);

/** * @brief reads a string and convert it into metadata. * * @param line: contains fields of fieldInfo.
* @return Metadata for fields in csv file if formatted correctly. A good metadata line should look like
field1/type1, field2/type2, field3/type3.... parseLineMetada should handle possible extra spaces on both ends
or between. otherwise it will throw InvalidMetadata exception. */
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

    /** * @brief processes a next valid line that passes the filter test. * @return vector<any> containing data 
    in a next valid line. Otherwise, return nullopt. */
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
    /** * @brief Constructs a new Csv File Scanner object to read lines in a csv file. * * @param metadataFileName: 
    name of the file that contains metadata about csv file. * @param dataFileName: name of the csv file that this 
    CsvFileScanner will read. * @param expr: Expression to filter desired lines. if not specified, passes every line. */
    CsvFileScanner(const std::string& metadataFileName,
        const std::string& dataFileName, const Expression& filterExpr = kAlwaysTrue);

    // helper function for processNext to check if there are too many error lines.
    void checkError();

    // const threshold to decide if there are too many error lines.
    const unsigned int kThreshold = 30;

    // metadata about the csv file.
    Metadata metadata_;
    // name of the csv file name.
    std::string dataFileName_;
    // file stream to open and read csv file and metadata file.
    mutable std::fstream dfs_;
    // expression based on which this object will filter lines.
    const Expression filterExpr_;
    // number of lines read by this object at a moment.
    unsigned int readLines_;
    // number of error lines that were discrepant with metadata_.
    unsigned int errorLines_;
};

/** * @brief Invalid Metadata exception for when metadata file contains an non-existent type name. */
class InvalidMetadata {};

/** * @brief exception for when CsvFileScanner cannot find a specified file. */
class NonExistentFile {};

/** * @brief exception for when there are too many error lines and thus metadata can be considered invalid
for csv file. */
class WrongMetadata {};

} // namespace codein