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
    while (true)
    {
        string code;
        string line;

        cout << "MathLang" << endl;
        cout << "Enter your code. Type 'done;' to execute." << endl;
        cout << "----------------------------------------" << endl;

        // MathLang 코드 입력
        while (true)
        {
            getline(cin, line);

            if (line == "done;")
                break;
            if (line == "stop;")
                return 0;

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

            // 3. Interpreter
            Interpreter interpreter;
            interpreter.evaluate(tree);
        }
        catch (const exception& e)
        {
            cerr << "Error : " << e.what() << endl;
        }

        // 실행이 끝난 후 Enter 대기
        cout << endl;
        cout << "Press Enter to continue...";
        getline(cin, line);

        cout << endl;
    }

    return 0;
}
