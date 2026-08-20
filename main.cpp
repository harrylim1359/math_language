#include <iostream>

#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

using namespace std;
using ll = long long;

int main()
{
    string code = "\\print(\"Hello, World\")";

    // 1. Lexer
    Lexer lexer(code);
    vector<Token> tokens = lexer.tokenize();

    // 2. Parser
    Parser parser(tokens);
    shared_ptr<ASTNode> tree = parser.parse();

    // AST 확인
    //tree->print();

    // 3. Interpreter
    Interpreter interpreter;
    interpreter.evaluate(tree);

    return 0;
}

