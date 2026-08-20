#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;
using ll = long long;

class ASTNode;

enum class ValueType
{
    Number,
    String,
    Boolean,
    Function,
    Array
};


class Value
{
public:

    ValueType type;

    bool isreturn = false;
    ll number=0;
    string str="";
    bool boolean=false;
    vector<string> parameters;
    vector<shared_ptr<ASTNode>> body;


    Value()
    {
        type = ValueType::Number;
        number = 0;
    }

    Value(ll n)
    {
        type = ValueType::Number;
        number = n;
    }

    Value(ll n, bool isrtn)
    {
        type = ValueType::Number;
        number = n;
        isreturn=isrtn;
    }

    Value(string s)
    {
        type = ValueType::String;
        str = s;
    }

    Value(string s, bool isrtn)
    {
        type = ValueType::String;
        str = s;
        isreturn=isrtn;
    }

    Value(bool b)
    {
        type = ValueType::Boolean;
        boolean = b;
    }

    Value(vector<string> paras, vector<shared_ptr<ASTNode>> sixpack)
    {
        type = ValueType::Function;
        parameters=paras;
        body=sixpack;
    }

    Value(vector<string> paras, vector<shared_ptr<ASTNode>> sixpack, bool isrtn)
    {
        type = ValueType::Function;
        parameters=paras;
        body=sixpack;
        isreturn=isrtn;
    }

    bool toBool() const
    {
        return boolean;
    }

    void print()
    {
        if(type == ValueType::Number)
        {
            cout << number;
        }
        else if(type == ValueType::String)
        {
            cout << str;
        }
        else if(type == ValueType::Boolean)
        {
            cout << (boolean ? "true" : "false");
        }
    }
};


#endif
