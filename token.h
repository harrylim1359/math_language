#ifndef TOKEN_H
#define TOKEN_H

#include <string>
using namespace std;
using ll = long long;

enum class TokenType
{
    NewLine,

    Number,
    Identifier,
    String,
    Boolean,

    Plus,
    Minus,
    Multiply,
    Divide,
    Modulo,

    Assign,

    NotEqual,
    Equal,
    Less,
    Greater,
    LessEqual,
    GreaterEqual,
    And,
    Or,
    Not,

    Print,

    If,
    Else,

    Repeat,

    Arrow,

    LeftBrace,
    RightBrace,

    LeftParen,
    RightParen,

    Comma,
    UnderBar,

    Function,
    Return,

    End
};


struct Token
{
    TokenType type;
    string value;

    ll line;
    ll column;

    Token(TokenType t, string v, ll l, ll c)
        : type(t), value(v), line(l), column(c)
    {}
};

#endif
