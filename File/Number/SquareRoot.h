#pragma once

#include "Fraction.h"

#include <climits>
#include <cmath>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <numeric>
#include <vector>

// Represents coefficient * root(degree, radicand).
// The SquareRoot name is retained for compatibility; root() also supports
// cube roots and other positive integer degrees.
class SquareRoot
{
public:
    using Coefficient = Fraction<long long>;

private:
    Coefficient coefficient{1};
    unsigned degree = 2;
    long long radicand = 1;

    static long long checkedMultiply(long long a, long long b)
    {
        if (a < 0 || b < 0)
            throw std::invalid_argument("Expected nonnegative values.");
        if (a != 0 && b > LLONG_MAX / a)
            throw std::overflow_error("Square-root radicand overflow.");
        return a * b;
    }

    static long long checkedPower(long long base, unsigned exponent)
    {
        long long result = 1;
        while (exponent > 0)
        {
            if ((exponent & 1U) != 0)
                result = checkedMultiply(result, base);
            exponent >>= 1U;
            if (exponent > 0)
                base = checkedMultiply(base, base);
        }
        return result;
    }

    void simplify()
    {
        if (degree < 2)
            throw std::invalid_argument("Root degree must be at least 2.");
        if (radicand < 0)
        {
            if ((degree & 1U) == 0)
                throw std::invalid_argument("Even root requires a nonnegative radicand.");
            if (radicand == LLONG_MIN)
                throw std::overflow_error("Root radicand overflow.");
            coefficient = Coefficient(0) - coefficient;
            radicand = -radicand;
        }
        if (radicand == 0 || coefficient == Coefficient(0))
        {
            coefficient = Coefficient(0);
            radicand = 1;
            return;
        }

        long long remaining = radicand;
        std::vector<std::pair<long long, unsigned>> factors;
        for (long long factor = 2; factor <= remaining / factor; ++factor)
        {
            unsigned count = 0;
            while (remaining % factor == 0)
            {
                remaining /= factor;
                ++count;
            }
            if (count != 0)
                factors.emplace_back(factor, count);
        }
        if (remaining > 1)
            factors.emplace_back(remaining, 1);

        // Reduce root(d, product(p^count)) by gcd(d, all prime exponents).
        // For example, root(4, 4) becomes root(2, 2).
        if (!factors.empty())
        {
            unsigned common = degree;
            for (const auto& factor : factors)
                common = std::gcd(common, factor.second);
            degree /= common;
            for (auto& factor : factors)
                factor.second /= common;
        }

        long long outside = 1;
        long long inside = 1;
        for (const auto& factor : factors)
        {
            const long long prime = factor.first;
            const unsigned count = factor.second;
            outside = checkedMultiply(outside, checkedPower(prime, count / degree));
            if (count % degree != 0)
                inside = checkedMultiply(inside, checkedPower(prime, count % degree));
        }

        coefficient *= Coefficient(outside);
        radicand = inside;
    }

    SquareRoot(Coefficient factor, unsigned rootDegree, long long value)
        : coefficient(std::move(factor)), degree(rootDegree), radicand(value)
    { simplify(); }

    static SquareRoot fromIntegerRoot(unsigned rootDegree, long long value)
    { return SquareRoot(Coefficient(1), rootDegree, value); }

    static SquareRoot fromFractionRoot(unsigned rootDegree, const Coefficient& value)
    {
        const long long numerator = value.getNumerator();
        const long long denominator = value.getDenominator();
        const bool negative = numerator < 0;
        if (negative && (rootDegree & 1U) == 0)
            throw std::invalid_argument("Even root requires a nonnegative value.");
        if (numerator == LLONG_MIN)
            throw std::overflow_error("Root numerator overflow.");

        const long long magnitude = negative ? -numerator : numerator;
        const long long denominatorPower = checkedPower(denominator, rootDegree - 1);
        const long long transformedRadicand = checkedMultiply(magnitude, denominatorPower);
        const Coefficient transformedCoefficient(negative ? -1 : 1, denominator);
        return SquareRoot(transformedCoefficient, rootDegree, transformedRadicand);
    }

public:
    SquareRoot() = default;
    explicit SquareRoot(long long value) : coefficient(value), degree(2), radicand(1) {}

    // Backward-compatible constructor: coefficient * sqrt(radicand).
    SquareRoot(Coefficient factor, long long value)
        : coefficient(std::move(factor)), degree(2), radicand(value)
    { simplify(); }

    static SquareRoot sqrt(long long value)
    { return fromIntegerRoot(2, value); }

    static SquareRoot sqrt(unsigned rootDegree, long long value)
    { return fromIntegerRoot(rootDegree, value); }

    static SquareRoot cbrt(long long value)
    { return fromIntegerRoot(3, value); }

    static SquareRoot sqrt(const Coefficient& value)
    { return fromFractionRoot(2, value); }

    static SquareRoot cbrt(const Coefficient& value)
    { return fromFractionRoot(3, value); }

    // Combines nested roots when the inner value has no outside coefficient:
    // root(outerDegree, root(innerDegree, n)) = root(outerDegree*innerDegree, n).
    static SquareRoot sqrt(unsigned outerDegree, const SquareRoot& value)
    {
        if (outerDegree < 2)
            throw std::invalid_argument("Root degree must be at least 2.");
        if (value.coefficient != Coefficient(1))
            throw std::invalid_argument(
                "Nested root simplification requires an inner coefficient of 1.");
        if (outerDegree > UINT_MAX / value.degree)
            throw std::overflow_error("Nested root degree overflow.");
        return fromIntegerRoot(outerDegree * value.degree, value.radicand);
    }

    const Coefficient& getCoefficient() const { return coefficient; }
    unsigned getDegree() const { return degree; }
    long long getRadicand() const { return radicand; }
    bool isRational() const { return radicand == 1; }

    double aprox() const
    {
        return coefficient.aprox()
             * std::pow(static_cast<double>(radicand), 1.0 / degree);
    }

    std::string toString() const
    {
        std::ostringstream out;
        if (radicand == 1)
        {
            out << coefficient;
            return out.str();
        }

        const std::string rootText = degree == 2
            ? "sqrt(" + std::to_string(radicand) + ")"
            : "root(" + std::to_string(degree) + "," + std::to_string(radicand) + ")";
        if (coefficient == Coefficient(1))
            return rootText;
        if (coefficient == Coefficient(-1))
            return "-" + rootText;
        out << coefficient << rootText;
        return out.str();
    }

    bool operator==(const SquareRoot& other) const
    {
        return coefficient == other.coefficient
            && degree == other.degree
            && radicand == other.radicand;
    }
    bool operator!=(const SquareRoot& other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream& out, const SquareRoot& value)
    { return out << value.toString(); }
};
