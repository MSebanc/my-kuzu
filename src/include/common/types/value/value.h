#pragma once

#include <utility>

#include "common/api.h"
#include "common/type_utils.h"
#include "common/types/date_t.h"
#include "common/types/internal_id_t.h"
#include "common/types/interval_t.h"
#include "common/types/ku_list.h"
#include "common/types/timestamp_t.h"

namespace kuzu {
namespace common {

class NodeVal;
class RelVal;
class FileInfo;
class NestedVal;
class RecursiveRelVal;
class ArrowRowBatch;
class ValueVector;

class Value {
    friend class NodeVal;
    friend class RelVal;
    friend class NestedVal;
    friend class RecursiveRelVal;
    friend class ArrowRowBatch;
    friend class ValueVector;

public:
    /**
     * @return a NULL value of ANY type.
     */
    KUZU_API static Value createNullValue();
    /**
     * @param dataType the type of the NULL value.
     * @return a NULL value of the given type.
     */
    KUZU_API static Value createNullValue(LogicalType dataType);
    /**
     * @param dataType the type of the non-NULL value.
     * @return a default non-NULL value of the given type.
     */
    KUZU_API static Value createDefaultValue(const LogicalType& dataType);
    /**
     * @param val_ the boolean value to set.
     * @return a Value with BOOL type and val_ value.
     */
    KUZU_API explicit Value(bool val_);
    /**
     * @param val_ the int16_t value to set.
     * @return a Value with INT16 type and val_ value.
     */
    KUZU_API explicit Value(int8_t val_);

    KUZU_API explicit Value(int16_t val_);
    /**
     * @param val_ the int32_t value to set.
     * @return a Value with INT32 type and val_ value.
     */
    KUZU_API explicit Value(int32_t val_);
    /**
     * @param val_ the int64_t value to set.
     * @return a Value with INT64 type and val_ value.
     */
    KUZU_API explicit Value(int64_t val_);
    /**
     * @param val_ the double value to set.
     * @return a Value with DOUBLE type and val_ value.
     */
    KUZU_API explicit Value(double val_);
    /**
     * @param val_ the float value to set.
     * @return a Value with FLOAT type and val_ value.
     */
    KUZU_API explicit Value(float_t val_);
    /**
     * @param val_ the date value to set.
     * @return a Value with DATE type and val_ value.
     */
    KUZU_API explicit Value(date_t val_);
    /**
     * @param val_ the timestamp value to set.
     * @return a Value with TIMESTAMP type and val_ value.
     */
    KUZU_API explicit Value(timestamp_t val_);
    /**
     * @param val_ the interval value to set.
     * @return a Value with INTERVAL type and val_ value.
     */
    KUZU_API explicit Value(interval_t val_);
    /**
     * @param val_ the internalID value to set.
     * @return a Value with INTERNAL_ID type and val_ value.
     */
    KUZU_API explicit Value(internalID_t val_);
    /**
     * @param val_ the string value to set.
     * @return a Value with STRING type and val_ value.
     */
    KUZU_API explicit Value(const char* val_);
    /**
     * @param val_ the string value to set.
     * @return a Value with type and val_ value.
     */
    KUZU_API explicit Value(LogicalType type, const std::string& val_);
    /**
     * @param vals the list value to set.
     * @return a Value with dataType type and vals value.
     */
    KUZU_API explicit Value(LogicalType dataType, std::vector<std::unique_ptr<Value>> children);
    /**
     * @param val_ the value to set.
     * @return a Value with dataType type and val_ value.
     */
    KUZU_API explicit Value(LogicalType dataType, const uint8_t* val_);
    /**
     * @param other the value to copy from.
     * @return a Value with the same value as other.
     */
    KUZU_API Value(const Value& other);
    /**
     * @brief Sets the data type of the Value.
     * @param dataType_ the data type to set to.
     */
    KUZU_API void setDataType(const LogicalType& dataType_);
    /**
     * @return the dataType of the value.
     */
    KUZU_API LogicalType* getDataType() const;
    /**
     * @brief Sets the null flag of the Value.
     * @param flag null value flag to set.
     */
    KUZU_API void setNull(bool flag);
    /**
     * @brief Sets the null flag of the Value to true.
     */
    KUZU_API void setNull();
    /**
     * @return whether the Value is null or not.
     */
    KUZU_API bool isNull() const;
    /**
     * @brief Copies from the value.
     * @param value value to copy from.
     */
    KUZU_API void copyValueFrom(const uint8_t* value);
    /**
     * @brief Copies from the other.
     * @param other value to copy from.
     */
    KUZU_API void copyValueFrom(const Value& other);
    /**
     * @return the value of the given type.
     */
    KUZU_API template<class T>
    T getValue() const {
        throw std::runtime_error("Unimplemented template for Value::getValue()");
    }
    /**
     * @return a reference to the value of the given type.
     */
    KUZU_API template<class T>
    T& getValueReference() {
        throw std::runtime_error("Unimplemented template for Value::getValueReference()");
    }
    /**
     * @param value the value to Value object.
     * @return a Value object based on value.
     */
    KUZU_API template<class T>
    static Value createValue(T value) {
        throw std::runtime_error("Unimplemented template for Value::createValue()");
    }
    /**
     * @return a copy of the current value.
     */
    KUZU_API std::unique_ptr<Value> copy() const;
    /**
     * @return the current value in string format.
     */
    KUZU_API std::string toString() const;

    void serialize(FileInfo* fileInfo, uint64_t& offset) const;

    static std::unique_ptr<Value> deserialize(FileInfo* fileInfo, uint64_t& offset);

private:
    Value();
    explicit Value(LogicalType dataType);

    void copyFromFixedList(const uint8_t* fixedList);
    void copyFromVarList(ku_list_t& list, const LogicalType& childType);
    void copyFromStruct(const uint8_t* kuStruct);
    void copyFromUnion(const uint8_t* kuUnion);

public:
    union Val {
        constexpr Val() : booleanVal{false} {}
        bool booleanVal;
        int64_t int64Val;
        int32_t int32Val;
        int16_t int16Val;
        int8_t int8Val;
        double doubleVal;
        float floatVal;
        interval_t intervalVal;
        internalID_t internalIDVal;
    } val;
    std::string strVal;

private:
    std::unique_ptr<LogicalType> dataType;
    bool isNull_;

    // Note: ALWAYS use childrenSize over children.size(). We do NOT resize children when iterating
    // with nested value. So children.size() reflects the capacity() rather the actual size.
    std::vector<std::unique_ptr<Value>> children;
    uint32_t childrenSize;
};

/**
 * @return boolean value.
 */
KUZU_API template<>
inline bool Value::getValue() const {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::BOOL);
    return val.booleanVal;
}

/**
 * @return int8 value.
 */
KUZU_API template<>
inline int8_t Value::getValue() const {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::INT8);
    return val.int8Val;
}

/**
 * @return int16 value.
 */
KUZU_API template<>
inline int16_t Value::getValue() const {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::INT16);
    return val.int16Val;
}

/**
 * @return int32 value.
 */
KUZU_API template<>
inline int32_t Value::getValue() const {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::INT32);
    return val.int32Val;
}

/**
 * @return int64 value.
 */
KUZU_API template<>
inline int64_t Value::getValue() const {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::INT64);
    return val.int64Val;
}

/**
 * @return float value.
 */
KUZU_API template<>
inline float Value::getValue() const {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::FLOAT);
    return val.floatVal;
}

/**
 * @return double value.
 */
KUZU_API template<>
inline double Value::getValue() const {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::DOUBLE);
    return val.doubleVal;
}

/**
 * @return date_t value.
 */
KUZU_API template<>
inline date_t Value::getValue() const {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::DATE);
    return date_t{val.int32Val};
}

/**
 * @return timestamp_t value.
 */
KUZU_API template<>
inline timestamp_t Value::getValue() const {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::TIMESTAMP);
    return timestamp_t{val.int64Val};
}

/**
 * @return interval_t value.
 */
KUZU_API template<>
inline interval_t Value::getValue() const {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::INTERVAL);
    return val.intervalVal;
}

/**
 * @return internal_t value.
 */
KUZU_API template<>
inline internalID_t Value::getValue() const {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::INTERNAL_ID);
    return val.internalIDVal;
}

/**
 * @return string value.
 */
KUZU_API template<>
inline std::string Value::getValue() const {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::STRING ||
           dataType->getLogicalTypeID() == LogicalTypeID::BLOB);
    return strVal;
}

/**
 * @return the reference to the boolean value.
 */
KUZU_API template<>
inline bool& Value::getValueReference() {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::BOOL);
    return val.booleanVal;
}

/**
 * @return the reference to the int8 value.
 */
KUZU_API template<>
inline int8_t& Value::getValueReference() {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::INT8);
    return val.int8Val;
}

/**
 * @return the reference to the int16 value.
 */
KUZU_API template<>
inline int16_t& Value::getValueReference() {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::INT16);
    return val.int16Val;
}

/**
 * @return the reference to the int32 value.
 */
KUZU_API template<>
inline int32_t& Value::getValueReference() {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::INT32);
    return val.int32Val;
}

/**
 * @return the reference to the int64 value.
 */
KUZU_API template<>
inline int64_t& Value::getValueReference() {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::INT64);
    return val.int64Val;
}

/**
 * @return the reference to the float value.
 */
KUZU_API template<>
inline float_t& Value::getValueReference() {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::FLOAT);
    return val.floatVal;
}

/**
 * @return the reference to the double value.
 */
KUZU_API template<>
inline double_t& Value::getValueReference() {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::DOUBLE);
    return val.doubleVal;
}

/**
 * @return the reference to the date value.
 */
KUZU_API template<>
inline date_t& Value::getValueReference() {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::DATE);
    return *reinterpret_cast<date_t*>(&val.int32Val);
}

/**
 * @return the reference to the timestamp value.
 */
KUZU_API template<>
inline timestamp_t& Value::getValueReference() {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::TIMESTAMP);
    return *reinterpret_cast<timestamp_t*>(&val.int64Val);
}

/**
 * @return the reference to the interval value.
 */
KUZU_API template<>
inline interval_t& Value::getValueReference() {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::INTERVAL);
    return val.intervalVal;
}

/**
 * @return the reference to the internal_id value.
 */
KUZU_API template<>
inline nodeID_t& Value::getValueReference() {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::INTERNAL_ID);
    return val.internalIDVal;
}

/**
 * @return the reference to the string value.
 */
KUZU_API template<>
inline std::string& Value::getValueReference() {
    assert(dataType->getLogicalTypeID() == LogicalTypeID::STRING);
    return strVal;
}

/**
 * @param val the boolean value
 * @return a Value with BOOL type and val value.
 */
KUZU_API template<>
inline Value Value::createValue(bool val) {
    return Value(val);
}

KUZU_API template<>
inline Value Value::createValue(int8_t val) {
    return Value(val);
}

/**
 * @param val the int16 value
 * @return a Value with INT16 type and val value.
 */
KUZU_API template<>
inline Value Value::createValue(int16_t val) {
    return Value(val);
}

/**
 * @param val the int32 value
 * @return a Value with INT32 type and val value.
 */
KUZU_API template<>
inline Value Value::createValue(int32_t val) {
    return Value(val);
}

/**
 * @param val the int64 value
 * @return a Value with INT64 type and val value.
 */
KUZU_API template<>
inline Value Value::createValue(int64_t val) {
    return Value(val);
}

/**
 * @param val the double value
 * @return a Value with DOUBLE type and val value.
 */
KUZU_API template<>
inline Value Value::createValue(double val) {
    return Value(val);
}

/**
 * @param val the date_t value
 * @return a Value with DATE type and val value.
 */
KUZU_API template<>
inline Value Value::createValue(date_t val) {
    return Value(val);
}

/**
 * @param val the timestamp_t value
 * @return a Value with TIMESTAMP type and val value.
 */
KUZU_API template<>
inline Value Value::createValue(timestamp_t val) {
    return Value(val);
}

/**
 * @param val the interval_t value
 * @return a Value with INTERVAL type and val value.
 */
KUZU_API template<>
inline Value Value::createValue(interval_t val) {
    return Value(val);
}

/**
 * @param val the nodeID_t value
 * @return a Value with NODE_ID type and val value.
 */
KUZU_API template<>
inline Value Value::createValue(nodeID_t val) {
    return Value(val);
}

/**
 * @param val the string value
 * @return a Value with type and val value.
 */
KUZU_API template<>
inline Value Value::createValue(std::string val) {
    return Value(LogicalType{LogicalTypeID::STRING}, val);
}

/**
 * @param val the string value
 * @return a Value with STRING type and val value.
 */
KUZU_API template<>
inline Value Value::createValue(const char* value) {
    return Value(LogicalType{LogicalTypeID::STRING}, std::string(value));
}

} // namespace common
} // namespace kuzu