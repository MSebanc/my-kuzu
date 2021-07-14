#pragma once

#include "src/processor/include/physical_plan/operator/read_list/frontier/frontier_set.h"
#include "src/processor/include/physical_plan/operator/read_list/read_list.h"

namespace graphflow {
namespace processor {

template<bool IS_OUT_DATACHUNK_FILTERED>
class FrontierExtend : public ReadList {

public:
    FrontierExtend(uint64_t inDataChunkPos, uint64_t inValueVectorPos, AdjLists* lists,
        uint64_t lowerBound, uint64_t upperBound, unique_ptr<PhysicalOperator> prevOperator,
        ExecutionContext& context, uint32_t id);

    void getNextTuples() override;

    unique_ptr<PhysicalOperator> clone() override;

private:
    bool computeFrontiers();
    void produceOutputTuples();
    FrontierBag* createFrontierBag();
    FrontierSet* createInitialFrontierSet();
    FrontierSet* makeFrontierSet(uint64_t layer);
    void createGlobalFrontierFromThreadLocalFrontiers(uint64_t layer);
    void extendToThreadLocalFrontiers(uint64_t layer);

private:
    uint64_t startLayer;
    uint64_t endLayer;
    vector<FrontierSet*> frontierPerLayer;
    vector<vector<FrontierBag*>> threadLocalFrontierPerLayer;
    // We create a vector and listHandle per thread assuming omp_get_max_threads() threads.
    // Each thread uses their vector and listHandle for uncoordinated extensions.
    vector<shared_ptr<NodeIDVector>> vectors;
    vector<unique_ptr<ListHandle>> listHandles;

    struct CurrentOutputPosition {
        bool hasMoreTuplesToProduce;
        uint64_t layer;
        uint64_t blockIdx;
        uint64_t slot;

        void reset(uint64_t startLayer) {
            hasMoreTuplesToProduce = false;
            layer = startLayer;
            blockIdx = slot = 0u;
        }
    } currOutputPos;
};

} // namespace processor
} // namespace graphflow
