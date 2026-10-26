#pragma once

#include "Fraction.h"

#include <climits>
#include <cmath>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

// Represents one exact value of the form coefficient * sqrt(radicand).
// This is a deliberately small starting point; sums of different roots and
// arithmetic between square-root expressions can be added separately later.
class SquareRoot
{
public:
    using Coefficient = Fraction<long long>;

private:
    Coefficient coefficient{1};
    long long radicand = 1;

    static long long integerPower(long long base, unsigned exponent)
    {
        long long result = 1;
        while (exponent-- > 0)
        {
            if (result > LLONG_MAX / base)
                throw std::overflow_error("Square-root simplification overflow.");
            result *= base;
        }
        return result;
    }

    void simplify()
    {
        if (radicand < 0)
            throw std::invalid_argument("Square root requires a nonnegative radicand.");
        if (radicand == 0 || coefficient == Coefficient(0))
        {
            coefficient = Coefficient(0);
            radicand = 1;
            return;
        }

        long long remaining = radicand;
        long long outside = 1;
        long long inside = 1;
        for (long long factor = 2; factor <= remaining / factor; ++factor)
        {
            unsigned count = 0;
            while (remaining % factor == 0)
            {
                remaining /= factor;
                ++count;
            }
            outside *= integerPower(factor, count / 2);
            if (count % 2 != 0)
                inside *= factor;
        }
        if (remaining > 1)
            inside *= remaining;

        coefficient *= Coefficient(outside);
        radicand = inside;
    }

public:
    SquareRoot() = default;
    explicit SquareRoot(long long value) : coefficient(value), radicand(1) {}
    SquareRoot(Coefficient factor, long long value)
        : coefficient(std::move(factor)), radicand(value)
    { simplify(); }

    static SquareRoot sqrt(long long value)
    { return SquareRoot(Coefficient(1), value); }

    static SquareRoot sqrt(const Coefficient& value)
    {
        if (value.getNumerator() < 0)
            throw std::invalid_argument("Square root requires a nonnegative value.");
        const long long numerator = value.getNumerator();
        const long long denominator = value.getDenominator();
        if (numerator == 0) return SquareRoot(0);
        if (numerator > LLONG_MAX / denominator)
            throw std::overflow_error("Square-root radicand overflow.");
        return SquareRoot(Coefficient(1, denominator), numerator * denominator);
    }

    const Coefficient& getCoefficient() const { return coefficient; }
    long long getRadicand() const { return radicand; }
    bool isRational() const { return radicand == 1; }

    double aprox() const
    { return coefficient.aprox() * std::sqrt(static_cast<double>(radicand)); }

    std::string toString() const
    {
        std::ostringstream out;
        if (radicand == 1)
        {
            out << coefficient;
            return out.str();
        }
        if (coefficient == Coefficient(1))
            return "sqrt(" + std::to_string(radicand) + ")";
        if (coefficient == Coefficient(-1))
            return "-sqrt(" + std::to_string(radicand) + ")";
        out << coefficient << "sqrt(" << radicand << ')';
        return out.str();
    }

    bool operator==(const SquareRoot& other) const
    { return coefficient == other.coefficient && radicand == other.radicand; }
    bool operator!=(const SquareRoot& other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream& out, const SquareRoot& value)
    { return out << value.toString(); }
};
