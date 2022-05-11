#pragma once

#include "src/function/aggregate/include/aggregate_function.h"
#include "src/processor/include/physical_plan/operator/physical_operator.h"
#include "src/processor/include/physical_plan/operator/source_operator.h"

using namespace graphflow::function;

namespace graphflow {
namespace processor {

class BaseAggregateScan : public PhysicalOperator, public SourceOperator {

public:
    BaseAggregateScan(unique_ptr<ResultSetDescriptor> resultSetDescriptor,
        vector<DataPos> aggregatesPos, vector<DataType> aggregateDataTypes,
        unique_ptr<PhysicalOperator> child, uint32_t id)
        : PhysicalOperator{move(child), id}, SourceOperator{move(resultSetDescriptor)},
          aggregatesPos{move(aggregatesPos)}, aggregateDataTypes{move(aggregateDataTypes)} {}

    BaseAggregateScan(unique_ptr<ResultSetDescriptor> resultSetDescriptor,
        vector<DataPos> aggregatesPos, vector<DataType> aggregateDataTypes, uint32_t id)
        : PhysicalOperator{id}, SourceOperator{move(resultSetDescriptor)},
          aggregatesPos{move(aggregatesPos)}, aggregateDataTypes{move(aggregateDataTypes)} {}

    PhysicalOperatorType getOperatorType() override { return AGGREGATE_SCAN; }

    shared_ptr<ResultSet> init(ExecutionContext* context) override;

    bool getNextTuples() override = 0;

    unique_ptr<PhysicalOperator> clone() override = 0;

protected:
    void writeAggregateResultToVector(
        ValueVector& vector, uint64_t pos, AggregateState* aggregateState);

protected:
    vector<DataPos> aggregatesPos;
    vector<DataType> aggregateDataTypes;
    vector<shared_ptr<ValueVector>> aggregateVectors;
};

} // namespace processor
} // namespace graphflow
