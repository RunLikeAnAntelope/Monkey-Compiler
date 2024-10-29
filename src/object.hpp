#pragma once
#include <string>
namespace Object {

typedef std::string ObjectType;
const std::string INTEGER_OBJ = "INTEGER";
struct IObject {
    virtual ObjectType Type();
    virtual std::string Inspect();
};

struct Integer : public IObject {
    long int m_value;
    ObjectType Type() override;
    std::string Inspect() override;
};
} // namespace Object
