#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "value.h"
#include "environment.h"

#include <memory>
#include <map>

using namespace std;
using ll = long long;


class Interpreter
{

private:
    Environment env;

public:

    Value evaluate(shared_ptr<ASTNode> node);

};


#endif
