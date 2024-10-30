#include "evaluator.hpp"
#include "object.hpp"
namespace Evaluator {
template <typename T>
    : std::unique_ptr<Object::IObject> Eval(std::unique_ptr<Ast::INode> node) {
    auto integer = dynamic_cast<Ast::IntegerLiteral *>(node.get());
    if (integer != nullptr) {
        return std::make_unique<Object::Integer>(
            Object::Integer(integer->m_value));
    }
}

} // namespace Evaluator
