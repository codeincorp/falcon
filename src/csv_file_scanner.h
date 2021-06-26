/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

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

/**
 * @brief Splits fields in a line, trimming extra spaces on both sides of each field.
 * 
 * @param line: Input line in which each field is supposed to be separated by comma.
 * @return vector<string> containing each field. 
 */
std::vector<std::string> parseLine(const std::string& line);

/**
 * @brief Converts string typeName into corresponding type_index. 
 * 
 * @param typeName: type name in string
 * @return type_index of typeName if the type is supported. otherwise, return type_index of void. 
 */
std::type_index convertToTypeid(const std::string& typeName);

/** 
 * @brief Reads a string and convert it into metadata.
 * 
 * @param line: contains fields of fieldInfo.
 * @return Metadata for fields in csv file if formatted correctly. A good metadata line should look like
 * field1/type1, field2/type2, field3/type3.... parseLineMetada should handle possible extra spaces 
 * on both ends or between. otherwise it will throw InvalidMetadata exception. 
 */
Metadata parseLineMetadata(const std::string& line);

/**
 * @brief CSV file scanner iterator. Scan the given CSV-formatted file.
 */
class CsvFileScanner : public Iterator {
public:
    template <typename T, typename... ArgTs>
    friend std::unique_ptr<Iterator> makeIterator(ArgTs&&...);

    void open() override {}

    void reopen() override
    {
        open();
        readLines_ = 0;
        errorLines_= 0;
    }

    bool hasNext() const override
    {
        return !dfs_.eof();
    }

    /** 
     * @brief Processes a next valid line that passes the filter test.
     * @returns converted data as vector inside optional. Otherwise, returns nullopt.
     */
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
     * @brief Constructs a new Csv File Scanner object to read lines in a CSV file. it sets
     *        projections_ to its default value so that it returns every column.
     * 
     * @param metadataFileName: name of the file that contains metadata about CSV file. 
     * @param dataFileName: name of the CSV file that this CsvFileScanner will read. 
     * @param filterExpr: Expression to filter desired lines. if not specified, passes every line.
     */
    CsvFileScanner(const std::string& metadataFileName,
        const std::string& dataFileName, const Expression& filterExpr = kAlwaysTrue);

    /**
     * @brief Constructor where arguments must specify values of filterExpr and projections.
     * 
     * @param metadataFileName: name of the metadata file.
     * @param dataFileName: name of the CSV file.
     * @param filterExpr: Expression to filter desired lines.
     * @param projections: projections to select desired columns or output in a line.
     */
    CsvFileScanner(const std::string& metadataFileName,
        const std::string& dataFileName, const Expression& filterExpr, 
        const std::vector<Expression>& projections);

    /**
     * @brief Helper function for constructors. it opens file stream to csv file and its metadata file.
     * 
     * @param metadataFileName: name of the metadata file, which will be passed from the calling constructor.
     * @param dataFileName: name of the csv file, which will be passed from the calling constructor.
     */
    void constructorHelper(const std::string& metadataFileName, const std::string& dataFileName);

    // helper function for processNext to check if there are too many error lines.
    void checkError();

    // threshold to decide if there are too many error lines.
    const unsigned int kThreshold = 30;

    // metadata about the CSV file.
    Metadata metadata_;
    // file stream to open and read CSV file and metadata file.
    mutable std::fstream dfs_;
    // expression based on which this object will filter lines.
    const Expression filterExpr_;
    // number of lines read by this object at a moment.
    unsigned int readLines_;
    // number of error lines that were discrepant with metadata_.
    unsigned int errorLines_;
    // projections to get desired columns
    std::vector<Expression> projections_;
};

/**
 * @brief Invalid Metadata exception for when metadata file contains a non-existent type name. 
 */
class InvalidMetadata {};

/**
 * @brief exception for when CsvFileScanner cannot find a specified file. 
 */
class NonExistentFile {};

/**
 * @brief exception for when there are too many error lines and thus metadata can be considered invalid
 * for CSV file. 
 */
class WrongMetadata {};

} // namespace codein
