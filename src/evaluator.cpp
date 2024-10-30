#include "evaluator.hpp"
#include "object.hpp"
#include <cassert>
namespace Evaluator {

std::unique_ptr<Object::IObject> Eval(std::unique_ptr<Ast::INode> node) {
    assert(node.get() != nullptr);
    auto integer = dynamic_cast<Ast::IntegerLiteral *>(node.get());
    if (integer != nullptr) {
        return std::make_unique<Object::Integer>(
            Object::Integer(integer->m_value));
    }
    return nullptr;
}

} // namespace Evaluator
