#include "interpreter.h"

using ll = long long;

Value Interpreter::evaluate(shared_ptr<ASTNode> node)
{

    // ProgramNode
    auto program =
    dynamic_pointer_cast<ProgramNode>(node);

    if(program)
    {
        Value result;

        for(auto statement : program->statements)
        {
            result = evaluate(statement);
        }

        return result;
    }

    // NumberNode
    auto num =
    dynamic_pointer_cast<NumberNode>(node);

    if(num)
    {
        return Value(num->value);
    }

    //BooleanNode
    auto boo=
    dynamic_pointer_cast<BooleanNode>(node);
    if(boo)
    {
        return Value(boo->value);
    }

    // StringNode
    auto str =
    dynamic_pointer_cast<StringNode>(node);

    if(str)
    {
        return Value(str->str);
    }



    // VariableNode
    auto var =
    dynamic_pointer_cast<VariableNode>(node);

    if(var)
    {
        if(!env.exists(var->name))
        {
            throw runtime_error("Undefined variable : " + var->name);
        }

        return env.get(var->name);
    }



    // PrintNode
    auto pri =
    dynamic_pointer_cast<PrintNode>(node);

    if(pri)
    {
        Value value = evaluate(pri->expression);

        value.print();

        return Value();
    }

    // UnaryNode
    auto una =
    dynamic_pointer_cast<UnaryNode>(node);

    if(una)
    {
        Value expre = evaluate(una->expression);

        if(una->op == "not")
        {
            if(expre.type != ValueType::Boolean)
            {
                throw runtime_error(
                    "'not' requires boolean"
                );
            }

            return Value(!expre.boolean);
        }
        if(una->op == "return")
        {
            if(expre.type==ValueType::Number)
            {
                return Value(expre.number, true);
            }
            if(expre.type==ValueType::Boolean)
            {
                return Value(expre.boolean, true);
            }
            if(expre.type==ValueType::String)
            {
                return Value(expre.str, true);
            }
            if(expre.type==ValueType::Function)
            {
                return Value(expre.parameters, expre.body, true);
            }
        }
    }

    // BinaryNode
    auto bin =
    dynamic_pointer_cast<BinaryNode>(node);

    if(bin)
    {

        // ´ëÀÔ
        if(bin->op == "=")
        {
            auto left =
            dynamic_pointer_cast<VariableNode>(bin->left);


            if(!left)
            {
                throw runtime_error("Left side of assignment must be variable");
            }


            Value value =
            evaluate(bin->right);


            env.set(left->name,value);


            return value;
        }



        // °è»ê
        // 계산
        Value left =
        evaluate(bin->left);

        Value right =
        evaluate(bin->right);


        // 산술 연산
        if(bin->op == "-" ||
        bin->op == "*" ||
        bin->op == "/")
        {
            if(left.type != ValueType::Number ||
            right.type != ValueType::Number)
            {
                throw runtime_error(
                    "Arithmetic requires numbers"
                );
            }
        }


        if(bin->op=="+")
        {
            if((left.type != ValueType::Number ||
            right.type != ValueType::Number) &&
            (left.type != ValueType::String ||
            right.type != ValueType::String))
            {
                //에러
            }
        }

        // 비교 연산
        if(bin->op == "<"  ||
        bin->op == ">"  ||
        bin->op == "<=" ||
        bin->op == ">=")
        {
            if(left.type != ValueType::Number ||
            right.type != ValueType::Number)
            {
                throw runtime_error(
                    "Comparison requires numbers"
                );
            }
        }

        if(bin->op == "==")
        {
            if(left.type != right.type)
            {
                return Value(false);
            }

            if(left.type == ValueType::Number)
                return Value(left.number == right.number);

            if(left.type == ValueType::Boolean)
                return Value(left.boolean == right.boolean);

            if(left.type == ValueType::String)
                return Value(left.str == right.str);

            throw runtime_error("Unsupported type for ==");
        }

        // 논리 연산
        if(bin->op == "and" ||
        bin->op == "or")
        {
            if(left.type != ValueType::Boolean ||
            right.type != ValueType::Boolean)
            {
                throw runtime_error(
                    "Logical operators require booleans"
                );
            }
        }



        if(bin->op == "+")
        {
            return Value(left.number + right.number);
        }


        if(bin->op == "-")
        {
            return Value(left.number - right.number);
        }


        if(bin->op == "*")
        {
            return Value(left.number * right.number);
        }

        if(bin->op == "%")
        {
            return Value(left.number % right.number);
        }

        if(bin->op == "/")
        {
            return Value(left.number / right.number);
        }
        if(bin->op == "<")
        {
            return Value(left.number < right.number);
        }
        if(bin->op == ">")
        {
            return Value(left.number > right.number);
        }
        if(bin->op == "<=")
        {
            return Value(left.number <= right.number);
        }
        if(bin->op == ">=")
        {
            return Value(left.number >= right.number);
        }
        if(bin->op == "and")
        {
            return Value(left.boolean && right.boolean);
        }
        if(bin->op == "or")
        {
            return Value(left.boolean || right.boolean);
        }
        if(bin->op=="~=")
        {
            return Value(left.number!=right.number);
        }
    }
    // IfNode
    auto Ifn = dynamic_pointer_cast<IfNode>(node);

    if(Ifn)
    {
        for(auto& branch : Ifn->branches)
        {
            // else
            if(branch.condition == nullptr)
            {
                for(auto& stmt : branch.body)
                {
                    evaluate(stmt);
                }

                break;
            }

            // if / else if
            if(evaluate(branch.condition).toBool())
            {
                for(auto& stmt : branch.body)
                {
                    evaluate(stmt);
                }

                break;
            }
        }

        return Value();
    }

    auto rpn = dynamic_pointer_cast<RepeatNode>(node);
    if(rpn)
    {
        auto& branch = rpn->branch;
        while(evaluate(branch.condition).toBool())
        {
            for(auto& stmt : branch.body)
            {
                evaluate(stmt);
            }
        }
        return Value();
    }

    auto func = dynamic_pointer_cast<FunctionNode>(node);
    if(func)
    {
        Value funct=Value(func->parameters, func->body);
        env.set(func->name,funct);
        return funct;
    }

    auto clfn = dynamic_pointer_cast<CallFunctionNode>(node);
    if(clfn)
    {
        Value funct=env.get(clfn->name);
        if(funct.parameters.size()!=clfn->parameters.size())
        {
            //에러
        }
        for(int i=0;i<funct.parameters.size();i++)
        {
            env.set(funct.parameters[i],evaluate(clfn->parameters[i]));
        }
        Value retrn;
        for(auto statement : funct.body)
        {
            Value eval = evaluate(statement);
            if(eval.isreturn)
            {
                if(eval.type==ValueType::Number)
                {
                    return Value(eval.number);
                }
                if(eval.type==ValueType::Boolean)
                {
                    return Value(eval.boolean);
                }
                if(eval.type==ValueType::String)
                {
                    return Value(eval.str);
                }
                if(eval.type==ValueType::Function)
                {
                    return Value(eval.parameters, eval.body);
                }
                break;
            }
        }
        return Value(); //임시 나중에 return 추가하면 수정
    }

    throw runtime_error("Unknown AST Node");

}
