#pragma once
#include "ast.hpp"
#include "object.hpp"
#include <memory>

namespace Evaluator {
class Evaluator {
  public:
    std::unique_ptr<Object::IObject> Eval(Ast::INode *node);

  private:
    std::unique_ptr<Object::IObject> TRUE =
        std::make_unique<Object::Boolean>(true);
    std::unique_ptr<Object::IObject> FALSE =
        std::make_unique<Object::Boolean>(false);
    std::unique_ptr<Object::IObject> NULL_OBJ =
        std::make_unique<Object::Null>();

    std::unique_ptr<Object::IObject>
    evalStatements(std::vector<std::unique_ptr<Ast::IStatement>> &stmts);
};

} // namespace Evaluator
