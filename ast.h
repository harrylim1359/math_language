#ifndef AST_H
#define AST_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include "value.h"

using namespace std;
using ll = long long;


// 모든 AST의 부모
class ASTNode
{
public:
    virtual void print(ll depth = 0) = 0;
    virtual ~ASTNode(){}
};


// 숫자
class NumberNode : public ASTNode
{
public:
    ll value;

    NumberNode(ll v)
    {
        value = v;
    }


    void print(ll depth = 0) override
    {
        for(ll i=0;i<depth;i++)
            cout << "  ";

        cout << "Number : " << value << endl;
    }
};


// 변수
class VariableNode : public ASTNode
{
public:

    string name;


    VariableNode(string n)
    {
        name=n;
    }


    void print(ll depth=0) override
    {
        for(ll i=0;i<depth;i++)
            cout<<"  ";

        cout<<"Variable : "<<name<<endl;
    }
};



// + - * / 같은 연산
class BinaryNode : public ASTNode
{
public:

    string op;

    shared_ptr<ASTNode> left;
    shared_ptr<ASTNode> right;



    BinaryNode(
        string o,
        shared_ptr<ASTNode> l,
        shared_ptr<ASTNode> r
    )
    {
        op=o;
        left=l;
        right=r;
    }



    void print(ll depth=0) override
    {
        for(ll i=0;i<depth;i++)
            cout<<"  ";

        cout<<"Operator : "<<op<<endl;


        left->print(depth+1);
        right->print(depth+1);
    }
};

class PrintNode : public ASTNode
{
public:
    shared_ptr<ASTNode> expression;
    PrintNode(shared_ptr<ASTNode> expr)
    {
        expression = expr;
    }
    void print(ll depth=0) override
    {
        for(ll i=0;i<depth;i++) cout<<"  ";
        cout<<"print"<<endl;
        expression->print(depth+1);
    }
};

class StringNode : public ASTNode
{
public:
    string str;
    StringNode(string s)
    {
        str = s;
    }
    void print(ll depth=0) override
    {
        for(ll i=0;i<depth;i++) cout<<"  ";
        cout<<"String : "<< str << endl;
    }
};

class ProgramNode : public ASTNode
{
public:

    vector<shared_ptr<ASTNode>> statements;

    void print(ll depth=0) override
    {
        for(ll i=0;i<depth;i++)
            cout<<"  ";

        cout<<"Program"<<endl;

        for(auto node : statements)
        {
            node->print(depth+1);
        }
    }
};

class UnaryNode : public ASTNode
{
public:

    string op;

    shared_ptr<ASTNode> expression;


    UnaryNode(
        string o,
        shared_ptr<ASTNode> expr
    )
    {
        op = o;
        expression = expr;
    }


    void print(ll depth = 0) override
    {
        for(ll i = 0; i < depth; i++)
        {
            cout << "  ";
        }

        cout << "Unary : " << op << endl;

        expression->print(depth + 1);
    }
};

class BooleanNode : public ASTNode
{
public:

    bool value;

    BooleanNode(bool v)
    {
        value = v;
    }

    void print(ll depth = 0) override
    {
        for(ll i = 0; i < depth; i++)
            cout << "  ";

        cout << "Boolean : "
             << (value ? "true" : "false")
             << endl;
    }
};

struct Branch
{
    shared_ptr<ASTNode> condition;   // nullptr이면 else
    vector<shared_ptr<ASTNode>> body;
};

class IfNode : public ASTNode
{
public:
    vector<Branch> branches;


    void print(ll depth = 0) override
    {
        for(auto& branch : branches)
        {
            for(ll i=0;i<depth;i++)
                cout<<"  ";

            if(branch.condition)
            {
                cout<<"If"<<endl;
                branch.condition->print(depth+1);
            }
            else
            {
                cout<<"Else"<<endl;
            }

            for(auto& statement : branch.body)
            {
                statement->print(depth+1);
            }
        }
    }
};

class RepeatNode : public ASTNode
{
public:
    Branch branch;

    void print(ll depth=0) override
    {
        for(ll i=0;i<depth;i++)
                cout << "  ";

        cout << "Repeat" << endl;
        branch.condition->print(depth+1);
        for(auto& statement : branch.body)
        {
            statement->print(depth+1);
        }
    }
};


class FunctionNode : public ASTNode
{
public:
    string name;
    bool isMath=false;
    vector<string> parameters;
    vector<shared_ptr<ASTNode>> body;
    shared_ptr<ASTNode> MathExpression;

    void print(ll depth=0) override
    {
        for(ll i=0;i<depth;i++) cout << "  ";
        cout << "Function"<< endl;
        for(ll i=0;i<depth+1;i++) cout << "  ";
        cout <<name << endl;
        for(ll i=0;i<depth+1;i++) cout << "  ";
        cout << "parameters :";
        for(auto& parameter : parameters)
        {
            cout << parameter << " ";
        }
        cout << endl;
        for(ll i=0;i<depth+1;i++) cout << "  ";
        cout << "body" << endl;
        for(auto& statement : body)
        {
            statement->print(depth+1);
        }
    }
};

class CallFunctionNode : public ASTNode
{
public:
    string name;
    vector<shared_ptr<ASTNode>> parameters;
    void print(ll depth=0) override
    {
        for(ll i=0;i<depth;i++) cout << "  ";
        cout << "CallFunction"<< endl;
        for(ll i=0;i<depth+1;i++) cout << "  ";
        cout << "parameters :";
        for(auto& parameter : parameters)
        {
            parameter->print();
            cout << " ";
        }
        cout << endl;
    }
};

class SequenceNode : public ASTNode
{
public:
    string name;
    shared_ptr<BinaryNode> SquenceFunction;
    unordered_map<ll, ll> SequenceEditted;
    void print(ll depth=0) override
    {
        for(ll i=0;i<depth;i++) cout << "  ";
        cout << "Sequence" << endl;
        SquenceFunction->print(depth+1);
    }
};

#endif
