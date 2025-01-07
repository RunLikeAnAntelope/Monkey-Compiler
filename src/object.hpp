#pragma once
#include "ast.hpp"
#include <cstdint>
#include <memory>
#include <string>
namespace Object {

enum class ObjectType : std::uint8_t {
  INTEGER_OBJ,
  BOOLEAN_OBJ,
  NULL_OBJ,
  RETURN_VALUE_OBJ,
  ERROR_OBJ,
  FUNCTION_OBJ,
  STRING_OBJ,
  BUILTIN_OBJ
};
std::string objectTypeToStr(ObjectType type);
struct IObject {
  virtual ~IObject() = default;
  [[nodiscard]] virtual ObjectType Type() const = 0;
  [[nodiscard]] virtual std::string Inspect() const = 0;
};

class Environment {
public:
  struct EnvObj {
    std::shared_ptr<Object::IObject> obj;
    bool ok;
  };
  Environment() = default;
  explicit Environment(std::shared_ptr<Environment> outerEnv);
  EnvObj Get(const std::string &name);
  void Set(const std::string &name, std::shared_ptr<Object::IObject>);

  void PrintEnv();

private:
  std::shared_ptr<Environment> m_outerEnv;
  std::unordered_map<std::string, std::shared_ptr<Object::IObject>>
    m_environment;
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
  std::shared_ptr<IObject> m_value;
  explicit ReturnValue(std::shared_ptr<IObject> value);
  [[nodiscard]] ObjectType Type() const override;
  [[nodiscard]] std::string Inspect() const override;
};

struct Error : public IObject {
  std::string m_message;
  explicit Error(std::string message);
  [[nodiscard]] ObjectType Type() const override;
  [[nodiscard]] std::string Inspect() const override;
};
struct Function : public IObject {
  std::vector<std::unique_ptr<Ast::Identifier>> m_parameters;
  std::shared_ptr<Environment> m_env;
  std::unique_ptr<Ast::BlockStatement> m_body;
  Function(std::vector<std::unique_ptr<Ast::Identifier>> parameters,
           std::shared_ptr<Environment> env,
           std::unique_ptr<Ast::BlockStatement> body);
  [[nodiscard]] ObjectType Type() const override;
  [[nodiscard]] std::string Inspect() const override;
};

struct String : public IObject {
  std::string m_value;
  explicit String(std::string value);
  [[nodiscard]] ObjectType Type() const override;
  [[nodiscard]] std::string Inspect() const override;
};

using BuiltinFunction = void (*)(Object::IObject...);
struct Builtin : IObject {
  BuiltinFunction fn;
  [[nodiscard]] ObjectType Type() const override;
  [[nodiscard]] std::string Inspect() const override;
};

} // namespace Object
