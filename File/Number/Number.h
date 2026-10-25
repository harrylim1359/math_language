#pragma once

#include <string>

class Number
{
public:
    virtual ~Number() = default;

    virtual double aprox() const = 0;

    virtual std::string toString() const = 0;
};
