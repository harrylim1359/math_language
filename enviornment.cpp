#include "environment.h"

using ll = long long;
void Environment::set(string name, Value value)
{
    variables[name] = value;
}



Value Environment::get(string name)
{
    if(!exists(name))
    {
        throw runtime_error(
            "Undefined variable : " + name
        );
    }


    return variables[name];
}



bool Environment::exists(string name)
{
    return variables.find(name) != variables.end();
}
