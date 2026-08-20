#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include <vector>
#include <string>

using namespace std;
using ll = long long;

class Lexer
{
private:

    string code;

    ll pos;

    ll line;

    ll column;

public:

    Lexer(string input);

    vector<Token> tokenize();
};

#endif
