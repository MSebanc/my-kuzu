#include "src/processor/include/physical_plan/operator/scan_node_id/scan_node_id.h"

namespace graphflow {
namespace processor {

void ScanNodeID::initResultSet(const shared_ptr<ResultSet>& resultSet) {
    PhysicalOperator::initResultSet(resultSet);
    outDataChunk = this->resultSet->dataChunks[outDataPos.dataChunkPos];
    outValueVector = make_shared<ValueVector>(context.memoryManager, NODE, true /* isSequence */);
    outDataChunk->insert(outDataPos.valueVectorPos, outValueVector);
}

void ScanNodeID::reInitialize() {
    if (prevOperator) {
        prevOperator->reInitialize();
    }
}

void ScanNodeID::getNextTuples() {
    metrics->executionTime.start();
    if (prevOperator) {
        prevOperator->getNextTuples();
    }
    {
        unique_lock<mutex> lock{morsel->mtx};
        auto nodeIDValues = (nodeID_t*)(outValueVector->values);
        // Fill the first nodeID in the sequence.
        if (morsel->currNodeOffset >= morsel->numNodes) {
            // no more tuples to scan_node_id.
            nodeIDValues[0].label = morsel->label;
            nodeIDValues[0].offset = 0;
            outDataChunk->state->initOriginalAndSelectedSize(0u);
        } else {
            nodeIDValues[0].label = morsel->label;
            nodeIDValues[0].offset = morsel->currNodeOffset;
            outDataChunk->state->initOriginalAndSelectedSize(
                min(DEFAULT_VECTOR_CAPACITY, morsel->numNodes - morsel->currNodeOffset));
            morsel->currNodeOffset += outDataChunk->state->selectedSize;
        }
    }
    metrics->executionTime.stop();
    metrics->numOutputTuple.increase(outDataChunk->state->selectedSize);
}

} // namespace processor
} // namespace graphflow
