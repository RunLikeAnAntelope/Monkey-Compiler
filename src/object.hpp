#pragma once
#include <cstdint>
#include <memory>
#include <string>
namespace Object {
enum class ObjectType : std::uint8_t {
    INTEGER_OBJ,
    BOOLEAN_OBJ,
    NULL_OBJ,
    RETURN_VALUE_OBJ,
    ERROR_OBJ
};
std::string objectTypeToStr(ObjectType type);
struct IObject {
    virtual ~IObject() = default;
    [[nodiscard]] virtual ObjectType Type() const = 0;
    [[nodiscard]] virtual std::string Inspect() const = 0;
};

struct Integer : public IObject {
    long int m_value;
    explicit Integer(long int value);
    [[nodiscard]] ObjectType Type() const override;
    [[nodiscard]] std::string Inspect() const override;
};

struct Boolean : public IObject {
    bool m_value;
    explicit Boolean(bool value);
    [[nodiscard]] ObjectType Type() const override;
    [[nodiscard]] std::string Inspect() const override;
};

struct Null : public IObject {
    [[nodiscard]] ObjectType Type() const override;
    [[nodiscard]] std::string Inspect() const override;
};

struct ReturnValue : public IObject {
    std::unique_ptr<IObject> m_value;
    explicit ReturnValue(std::unique_ptr<IObject> value);
    [[nodiscard]] ObjectType Type() const override;
    [[nodiscard]] std::string Inspect() const override;
};

struct Error : public IObject {
    std::string m_message;
    explicit Error(std::string message);
    [[nodiscard]] ObjectType Type() const override;
    [[nodiscard]] std::string Inspect() const override;
};
} // namespace Object
