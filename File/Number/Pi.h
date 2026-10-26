#pragma once

#include "Fraction.h"

#include <cmath>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

class Pi
{
public:
    using Exponent = Fraction<long long>;

private:
    Exponent exponent;
    Exponent coefficient;

    Pi(Exponent power, Exponent factor)
        : exponent(power), coefficient(factor) {}

public:
    static Pi value(const Exponent& power = Exponent(1))
    { return Pi(power, Exponent(1)); }

    Pi operator+(const Pi& other) const
    {
        if (exponent != other.exponent)
            throw std::invalid_argument("Only pi powers with the same exponent can be added.");
        return Pi(exponent, coefficient + other.coefficient);
    }

    long double aprox() const
    {
        const long double power = static_cast<long double>(exponent.getNumerator())
                                / static_cast<long double>(exponent.getDenominator());
        const long double factor = static_cast<long double>(coefficient.getNumerator())
                                 / static_cast<long double>(coefficient.getDenominator());
        return factor * std::pow(std::acos(-1.0L), power);
    }

    std::string toString() const
    {
        if (coefficient == Exponent(0))
            return "0";
        const bool negative = coefficient.getNumerator() < 0;
        const Exponent magnitude = negative ? Exponent(0) - coefficient : coefficient;
        std::ostringstream out;
        if (negative)
            out << "-";
        if (magnitude != Exponent(1))
            out << magnitude;
        out << "pi";
        if (exponent != Exponent(1))
            out << "^(" << exponent << ")";
        return out.str();
    }

    friend std::ostream& operator<<(std::ostream& out, const Pi& value)
    { return out << value.toString(); }
};
