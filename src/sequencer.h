#include <vector>
#include <fstream>

#include "iterator.h"
#include "expression.h"

#pragma once

namespace codein {

/**
 * @brief Sequence iterator to read from multiple children sequentially that share common output data format. 
 */
class Sequencer : public Iterator {
public:
    template <typename T, typename... ArgTs>
    friend std::unique_ptr<Iterator> makeIterator(ArgTs&&...);

    void open()
    {
        children_[0]->open();
        indexOfCurChild = 0;
    }

    void reopen() override 
    {
        open();
    }

    bool hasMore() const override 
    {
        return indexOfCurChild < children_.size();
    }

    std::optional<std::vector<std::any>> processNext();

    void close() override
    {
        children_[indexOfCurChild]->close();
        indexOfCurChild = children_.size();
    }

    const Metadata& getMetadata() const override
    {
        return children_[0]->getMetadata();
    }

    ~Sequencer() override {}

private:
    void checkMetadata(const std::vector<std::unique_ptr<Iterator>>& children);
    /**
     * @brief Constructs a new Sequencer object
     * 
     * @param children: vector of children iterator.
     * Invariant: Children iterators must have same metadata in order to have equal output data format. 
     * Otherwise, Sequencer should throw an exception.
     */
    Sequencer(std::vector<std::unique_ptr<Iterator>>&& children);

    // sequence of children iterators.
    std::vector<std::unique_ptr<Iterator>> children_;
    // index variable to keep track of which file is being read.
    size_t indexOfCurChild;

};

/**
 * @brief Exception for when children iterators have different metadata, 
 * resulting in different output data format.
 */
class DiscrepantOutputData {};

} // namespace codein
