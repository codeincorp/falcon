#include "sequencer.h"

namespace codein {

void Sequencer::checkMetadata() 
{
    Metadata metadata = children_[0]->getMetadata();
    for (size_t i = 1; i < children_.size(); ++i) {
        if (children_[i]->getMetadata() != metadata) {
            throw DiscrepantOutputData();
        }
    }
}

Sequencer::Sequencer(std::vector<std::unique_ptr<Iterator>>&& children)
    : children_(std::move(children))
    , i(0)
{
    checkMetadata();
}

std::optional<std::vector<std::any>> Sequencer::processNext()
{
    if (!hasMore()) {
        return std::nullopt;
    }

    auto output = children_[i]->processNext();
    if (output == std::nullopt) {
        ++i;
        if (i == children_.size()) {
            return std::nullopt;
        }
        
        children_[i]->open();
        output = children_[i]->processNext();
    }

    return output;
}

}// namespace codein