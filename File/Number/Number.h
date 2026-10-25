#pragma once

#include <string>
#include <boost/multiprecision/cpp_dec_float.hpp>

class Number
{
public:
    using Decimal = boost::multiprecision::cpp_dec_float_100;
    virtual ~Number() = default;

    virtual double aprox() const = 0;
    virtual Decimal aprox100() const { return Decimal(aprox()); }

    double approx() const
    {
        return aprox();
    }

    virtual std::string toString() const = 0;
};
