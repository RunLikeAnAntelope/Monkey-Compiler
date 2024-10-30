#pragma once
#include <string>
namespace Object {

typedef std::string ObjectType;
const std::string INTEGER_OBJ = "INTEGER";
const std::string BOOLEAN_OBJ = "BOOLEAN";
const std::string NULL_OBJ = "NULL";
struct IObject {
    virtual ObjectType Type() = 0;
    virtual std::string Inspect() = 0;
    virtual ~IObject() = default;
};

struct Integer : public IObject {
    long int m_value;
    Integer(long int value);
    ObjectType Type() override;
    std::string Inspect() override;
};

struct Boolean : public IObject {
    bool m_value;
    Boolean(long int value);
    ObjectType Type() override;
    std::string Inspect() override;
};

struct Null : public IObject {
    ObjectType Type() override;
    std::string Inspect() override;
};
} // namespace Object
