#include <iostream>
#include <string>
#include <cstdio>

#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "\n종료하려면 Enter를 누르세요...";
        cin.get();
        return 1;
    }

    string filename = argv[1];

    FILE* file = fopen(filename.c_str(), "rb");

    if (file == nullptr)
    {
        cout << "\n종료하려면 Enter를 누르세요...";
        cin.get();
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    string code;

    if (size > 0)
    {
        code.resize(size);
        fread(&code[0], 1, size, file);
    }

    fclose(file);

    try
    {
        Lexer lexer(code);
        vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        shared_ptr<ASTNode> tree = parser.parse();

        Interpreter interpreter;
        interpreter.evaluate(tree);
    }
    catch (const exception& e)
    {
        cerr << "Error : " << e.what() << endl;
        cout << "\n종료하려면 Enter를 누르세요...";
        cin.get();
        return 1;
    }

    cout << "\n종료하려면 Enter를 누르세요...debug";
    cin.get();

    return 0;
}
