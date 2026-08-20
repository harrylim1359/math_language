#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

using namespace std;
using ll = long long;

int main()
{
    string code;
    string line;

    cout << "MathLang" << endl;
    cout << "Enter your code. Type 'done;' to execute." << endl;
    cout << "----------------------------------------" << endl;

    while (true)
    {
        getline(cin, line);

        // done;을 입력하면 입력 종료
        if (line == "done;")
            break;

        code += line;
        code += '\n';
    }

    try
    {
        // 1. Lexer
        Lexer lexer(code);
        vector<Token> tokens = lexer.tokenize();

        // 2. Parser
        Parser parser(tokens);
        shared_ptr<ASTNode> tree = parser.parse();

        // AST 확인
        // tree->print();

        // 3. Interpreter
        Interpreter interpreter;
        interpreter.evaluate(tree);
    }
    catch (const exception& e)
    {
        cerr << "Error : " << e.what() << endl;
        return 1;
    }

    return 0;
}
