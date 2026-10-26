#pragma once

#include "Fraction.h"

#include <cmath>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

class E
{
public:
    using Exponent = Fraction<long long>;

private:
    Exponent exponent;
    Exponent coefficient;

    E(Exponent power, Exponent factor)
        : exponent(power), coefficient(factor) {}

public:
    static E value(const Exponent& power = Exponent(1))
    { return E(power, Exponent(1)); }

    E operator+(const E& other) const
    {
        if (exponent != other.exponent)
            throw std::invalid_argument("Only e powers with the same exponent can be added.");
        return E(exponent, coefficient + other.coefficient);
    }

    long double aprox() const
    {
        const long double power = static_cast<long double>(exponent.getNumerator())
                                / static_cast<long double>(exponent.getDenominator());
        const long double factor = static_cast<long double>(coefficient.getNumerator())
                                 / static_cast<long double>(coefficient.getDenominator());
        return factor * std::pow(std::exp(1.0L), power);
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
        out << "e";
        if (exponent != Exponent(1))
            out << "^(" << exponent << ")";
        return out.str();
    }

    friend std::ostream& operator<<(std::ostream& out, const E& value)
    { return out << value.toString(); }
};
