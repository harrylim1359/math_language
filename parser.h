#ifndef PARSER_H
#define PARSER_H


#include "token.h"
#include "ast.h"
#include <vector>
#include <memory>

using namespace std;
using ll = long long;


class Parser
{

private:

    vector<Token> tokens;

    ll pos;


public:

    Parser(vector<Token> t);


    shared_ptr<ASTNode> parse();

    Token current();
    Token next();


private:

    shared_ptr<ASTNode> parseExpression();
    shared_ptr<ASTNode> parseTerm();
    shared_ptr<ASTNode> parseFactor();
    shared_ptr<ASTNode> parseAssignment();
    shared_ptr<ASTNode> parsePrint();
    shared_ptr<ASTNode> parseStatement();
    shared_ptr<ASTNode> parseComparison();
    shared_ptr<ASTNode> parseLogicalAnd();
    shared_ptr<ASTNode> parseLogicalNot();
    shared_ptr<ASTNode> parseLogicalOr();
    shared_ptr<ASTNode> parseIf();
    shared_ptr<ASTNode> parseRepeat();
    shared_ptr<ASTNode> parseReturn();
    shared_ptr<ASTNode> parseCall();
    bool isFunctionDefinition();
    bool isSequenceDefinition();
};


#endif
