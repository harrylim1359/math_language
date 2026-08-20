#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "value.h"

#include <map>
#include <string>

using namespace std;
using ll = long long;


class Environment
{
private:

    map<string, Value> variables;


public:

    void set(string name, Value value);


    Value get(string name);


    bool exists(string name);

};


#endif
