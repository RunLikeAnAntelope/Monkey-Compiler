#pragma once
#include <string>
namespace Object {
enum class ObjectType { INTEGER_OBJ, BOOLEAN_OBJ, NULL_OBJ };
std::string objectTypeToStr(ObjectType type);
struct IObject {
    virtual ObjectType Type() const = 0;
    virtual std::string Inspect() const = 0;
    virtual ~IObject() = default;
};

struct Integer : public IObject {
    long int m_value;
    Integer(long int value);
    ObjectType Type() const override;
    std::string Inspect() const override;
};

struct Boolean : public IObject {
    bool m_value;
    Boolean(bool value);
    ObjectType Type() const override;
    std::string Inspect() const override;
};

struct Null : public IObject {
    ObjectType Type() const override;
    std::string Inspect() const override;
};
} // namespace Object
