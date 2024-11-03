#pragma once
#include <string>
namespace Object {
enum class ObjectType { INTEGER_OBJ, BOOLEAN_OBJ, NULL_OBJ };
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
    Boolean(bool value);
    ObjectType Type() override;
    std::string Inspect() override;
};

struct Null : public IObject {
    ObjectType Type() override;
    std::string Inspect() override;
};
} // namespace Object
