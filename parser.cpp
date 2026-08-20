#include "parser.h"
using ll = long long;
Token Parser::current()
{
    return tokens[pos];
}

Token Parser::next()
{
    return tokens[pos++];
}

Parser::Parser(vector<Token> t)
{
    tokens=t;
    pos=0;
}

shared_ptr<ASTNode> Parser::parseFactor()
{
    Token left = next();
    shared_ptr<ASTNode> node;
    if(left.type==TokenType::LeftParen)
    {
        node = parseLogicalOr();
        if(next().type!=TokenType::RightParen)
        {
            throw runtime_error("Expected ')'");
        }
    }
    else if(left.type==TokenType::LeftBrace)
    {
        node = parseLogicalOr();
        if(next().type!=TokenType::RightBrace)
        {
            throw runtime_error("Expected '}'");
        }
    }
    else
    {
        if(left.type==TokenType::Number)
        {
            node = make_shared<NumberNode>(stoi(left.value));
        }
        else if(left.type==TokenType::Boolean)
        {
            if(left.value == "true") node = make_shared<BooleanNode>(true);
            else node = make_shared<BooleanNode>(false);
        }
        else if(left.type == TokenType::Identifier)
        {
            node = make_shared<VariableNode>(left.value);
        }
        else if(left.type==TokenType::String)
        {
            node = make_shared<StringNode>(left.value);
        }
        else
        {
            throw runtime_error("Invalid factor");
        }
    }
    return node;
}

shared_ptr<ASTNode> Parser::parseCall()
{
    auto node = parseFactor();

    while(current().type == TokenType::LeftParen)
    {
        auto variable = dynamic_pointer_cast<VariableNode>(node);

        if(!variable)
            throw runtime_error("Expected function name");

        next(); // (

        auto call = make_shared<CallFunctionNode>();
        call->name = variable->name;

        while(current().type != TokenType::RightParen)
        {
            call->parameters.push_back(parseLogicalOr());

            if(current().type == TokenType::Comma)
            {
                next();
            }
            else if(current().type != TokenType::RightParen)
            {
                throw runtime_error("Expected ',' or ')'");
            }
        }

        next(); // )

        node = call;
    }

    return node;
}

shared_ptr<ASTNode> Parser::parseTerm()
{
    shared_ptr<ASTNode> left = parseCall();

    while (current().type == TokenType::Multiply ||
           current().type == TokenType::Divide ||
           current().type == TokenType::Modulo)
    {
        Token op = next();

        shared_ptr<ASTNode> right = parseCall();

        left = make_shared<BinaryNode>(op.value, left, right);
    }

    return left;
}

shared_ptr<ASTNode> Parser::parseExpression()
{
    shared_ptr<ASTNode> left = parseTerm();

    while(current().type == TokenType::Plus ||
          current().type == TokenType::Minus)
    {
        Token op = next();
        shared_ptr<ASTNode> right = parseTerm();
        left = make_shared<BinaryNode>(op.value, left, right);
    }
    return left;
}

shared_ptr<ASTNode> Parser::parseComparison()
{
    shared_ptr<ASTNode> left = parseExpression();

    while(current().type==TokenType::Equal ||
       current().type == TokenType::Less ||
       current().type == TokenType::Greater ||
       current().type == TokenType::LessEqual ||
       current().type == TokenType::GreaterEqual ||
       current().type == TokenType::NotEqual)
    {
        Token op = next();
        shared_ptr<ASTNode> right = parseExpression();
        left = make_shared<BinaryNode>(op.value, left, right);
    }
    return left;
}

bool Parser::isFunctionDefinition()
{
    // 함수 이름이 아니면 함수 정의일 수 없음
    if(current().type != TokenType::Identifier)
        return false;

    // 현재 위치 다음이 '('인지 확인
    if(pos + 1 >= tokens.size() ||
       tokens[pos + 1].type != TokenType::LeftParen)
    {
        return false;
    }

    // '('부터 시작해서 대응하는 ')' 찾기
    ll i = pos + 1;
    ll depth = 0;

    while(i < tokens.size())
    {
        if(tokens[i].type == TokenType::LeftParen)
        {
            depth++;
        }
        else if(tokens[i].type == TokenType::RightParen)
        {
            depth--;

            // 처음 '('와 대응하는 ')'를 찾음
            if(depth == 0)
            {
                i++;
                break;
            }
        }

        i++;
    }

    // ')'를 못 찾음
    if(depth != 0)
        return false;

    // ')' 뒤의 개행은 허용
    while(i < tokens.size() &&
          tokens[i].type == TokenType::NewLine)
    {
        i++;
    }

    // ) 다음이 '=' 또는 '=>'이면 함수 정의
    if(i < tokens.size() &&
       (tokens[i].type == TokenType::Assign ||
        tokens[i].type == TokenType::Arrow))
    {
        return true;
    }

    return false;
}

bool Parser::isSequenceDefinition()
{
    ll startpos=pos;

    // 함수 이름이 아니면 함수 정의일 수 없음
    if(current().type != TokenType::Identifier)
        return false;

    // 현재 위치 다음이 '('인지 확인
    if(pos + 1 >= tokens.size() ||
       tokens[pos + 1].type != TokenType::UnderBar)
    {
        return false;
    }
    next();
    next();

    parseFactor();

    if(current().type == TokenType::Assign)
    {
        pos=startpos;
        return true;
    }
    return false;
}

shared_ptr<ASTNode> Parser::parseAssignment()
{
    // ==========================================
    // 함수 정의인지 먼저 확인
    // ==========================================
    if(isFunctionDefinition())
    {
        auto func = make_shared<FunctionNode>();

        // 함수 이름
        func->name = current().value;
        next(); // name

        next(); // (

        // 매개변수
        while(current().type != TokenType::RightParen)
        {
            if(current().type != TokenType::Identifier)
                throw runtime_error("Expected parameter name");

            func->parameters.push_back(current().value);
            next();

            if(current().type == TokenType::Comma)
            {
                next();
            }
            else if(current().type != TokenType::RightParen)
            {
                throw runtime_error("Expected ',' or ')'");
            }
        }

        next(); // )

        while(current().type == TokenType::NewLine)
            next();

        // ==========================================
        // 일반 함수
        // add(a,b) => { ... }
        // ==========================================
        if(current().type == TokenType::Arrow)
        {
            next(); // =>

            while(current().type == TokenType::NewLine)
                next();

            if(current().type != TokenType::LeftBrace)
                throw runtime_error("Expected '{'");

            next(); // {

            while(current().type != TokenType::RightBrace)
            {
                if(current().type == TokenType::NewLine)
                {
                    next();
                    continue;
                }

                func->body.push_back(parseStatement());
            }

            next(); // }

            return func;
        }

        // ==========================================
        // 수학 함수
        // f(x) = x*x
        // ==========================================
        if(current().type == TokenType::Assign)
        {
            next(); // =

            while(current().type == TokenType::NewLine)
                next();

            auto state = parseLogicalOr();

            auto unary =
                make_shared<UnaryNode>("return", state);

            func->body.push_back(unary);
            func->isMath=1;
            func->MathExpression=state;

            return func;
        }
    }

    // ==========================================
    // 여기부터는 일반적인 식
    // ==========================================

    shared_ptr<ASTNode> variable = parseLogicalOr();

    // 일반 변수 대입
    if(current().type == TokenType::Assign)
    {
        auto left =
            dynamic_pointer_cast<VariableNode>(variable);

        if(!left)
            throw runtime_error("Invalid assignment target");

        next(); // =

        auto right = parseAssignment();

        return make_shared<BinaryNode>("=", left, right);
    }

    return variable;
}

shared_ptr<ASTNode> Parser::parsePrint()
{
    next();
    shared_ptr<ASTNode> expr = parseLogicalOr();
    return make_shared<PrintNode>(expr);
}

shared_ptr<ASTNode> Parser::parseReturn()
{
    next();
    return make_shared<UnaryNode>("return", parseLogicalOr());
}

shared_ptr<ASTNode> Parser::parseStatement()
{
    if(current().type == TokenType::Print)
    {
        return parsePrint();
    }
    else if(current().type == TokenType::If)
    {
        return parseIf();
    }
    else if(current().type == TokenType::Repeat)
    {
        return parseRepeat();
    }
    else if(current().type == TokenType::Return)
    {
        return parseReturn();
    }

    return parseAssignment();
}

shared_ptr<ASTNode> Parser::parseLogicalNot()
{
    if(current().type == TokenType::Not)
    {
        next();
        return make_shared<UnaryNode>(
            "not",
            parseLogicalNot()
        );
    }

    return parseComparison();
}

shared_ptr<ASTNode> Parser::parseLogicalOr()
{
    auto left = parseLogicalAnd();

    while(current().type == TokenType::Or)
    {
        Token op = next();
        auto right = parseLogicalAnd();

        left = make_shared<BinaryNode>(op.value, left, right);
    }

    return left;
}

shared_ptr<ASTNode> Parser::parseLogicalAnd()
{
    auto left = parseLogicalNot();

    while(current().type == TokenType::And)
    {
        Token op = next();
        auto right = parseLogicalNot();

        left = make_shared<BinaryNode>(op.value, left, right);
    }

    return left;
}

shared_ptr<ASTNode> Parser::parseRepeat()
{
    auto node = make_shared<RepeatNode>();
    Branch branch;
    if(current().type == TokenType::Repeat)
    {
        next();
        branch.condition = parseLogicalOr();
    }
    while(current().type == TokenType::NewLine)
        next();
    if(current().type != TokenType::Arrow)
    {
        throw runtime_error("expected \"=>\"");
    }
    next();
    while(current().type == TokenType::NewLine)
        next();
    if(current().type != TokenType::LeftBrace)
        throw runtime_error("expected \"{\"");
    next();
    while(current().type != TokenType::RightBrace)
    {
        if(current().type == TokenType::NewLine)
        {
            next();
            continue;
        }

        branch.body.push_back(parseStatement());
    }
    next();
    node->branch = branch;
    while(current().type == TokenType::NewLine)
        next();
    return node;
}

shared_ptr<ASTNode> Parser::parseIf()
{
    auto node = make_shared<IfNode>();

    while(true)
    {
        Branch branch;

        // if
        if(current().type == TokenType::If)
        {
            next();
            branch.condition = parseLogicalOr();
        }
        // else 또는 else if
        else if(current().type == TokenType::Else)
        {
            next();

            if(current().type == TokenType::If)
            {
                next();
                branch.condition = parseLogicalOr();
            }
            else
            {
                // else
                branch.condition = nullptr;
            }
        }
        else
        {
            break;
        }

        // 조건 뒤 개행 허용
        while(current().type == TokenType::NewLine)
            next();

        if(current().type != TokenType::Arrow)
        {
            throw runtime_error("expected \"=>\"");
        }

        next();

        // => 뒤 개행 허용
        while(current().type == TokenType::NewLine)
            next();

        if(current().type != TokenType::LeftBrace)
            throw runtime_error("expected \"{\"");

        next();

        // 블록 읽기
        while(current().type != TokenType::RightBrace)
        {
            if(current().type == TokenType::NewLine)
            {
                next();
                continue;
            }

            branch.body.push_back(parseStatement());
        }

        next(); // }

        node->branches.push_back(branch);

        // } 뒤 개행 허용
        while(current().type == TokenType::NewLine)
            next();

        // else가 없으면 종료
        if(current().type != TokenType::Else)
            break;
    }

    return node;
}

shared_ptr<ASTNode> Parser::parse()
{
    auto program = make_shared<ProgramNode>();

    while(current().type != TokenType::End)
    {
        if(current().type == TokenType::NewLine)
        {
            next();
            continue;
        }

        auto node = parseStatement();
        program->statements.push_back(node);
    }

    return program;
}
