#pragma once

#include "RealNumber.h"
#include "Fraction.h"
#include "BigInteger.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <boost/multiprecision/cpp_dec_float.hpp>

class IrrationalNumber : public RealNumber
{
public:
    using Integer = BigInteger;
    using FractionValue = Fraction<Integer>;

private:
    struct RadicalTerm
    {
        FractionValue coefficient;
        Integer degree;
        Integer radicand;
    };

    FractionValue rationalPart{0};
    std::vector<RadicalTerm> radicalTerms;

    static Integer abs(Integer value) { return value < 0 ? -value : value; }

    static Integer gcd(Integer a, Integer b)
    {
        a = abs(a); b = abs(b);
        while (b != 0) { Integer r = a % b; a = b; b = r; }
        return a;
    }

    static Integer lcm(const Integer& a, const Integer& b)
    { return a == 0 || b == 0 ? Integer(0) : abs((a / gcd(a, b)) * b); }

    static Integer integerPower(Integer base, Integer exponent)
    {
        if (exponent < 0) throw std::invalid_argument("Negative integer power.");
        Integer result = 1;
        while (exponent > 0)
        {
            if ((exponent & 1) != 0) result *= base;
            exponent >>= 1;
            if (exponent > 0) base *= base;
        }
        return result;
    }

    static FractionValue fractionPower(FractionValue base, Integer exponent)
    {
        FractionValue result(1);
        while (exponent > 0)
        {
            if ((exponent & 1) != 0) result *= base;
            exponent >>= 1;
            if (exponent > 0) base *= base;
        }
        return result;
    }

    static std::pair<Integer, Integer> simplifyRadical(
        const Integer& degree, const Integer& value)
    {
        if (degree < 2) throw std::invalid_argument("Root degree must be at least 2.");
        if (value <= 0) throw std::invalid_argument("Radicand must be positive.");

        Integer remaining = value;
        Integer coefficient = 1;
        Integer radicand = 1;

        // Exact trial factorization keeps the historical fully simplified form.
        for (Integer prime = 2; prime <= remaining / prime; ++prime)
        {
            Integer count = 0;
            while (remaining % prime == 0) { remaining /= prime; ++count; }
            coefficient *= integerPower(prime, count / degree);
            radicand *= integerPower(prime, count % degree);
        }
        if (remaining > 1) radicand *= remaining;
        return {coefficient, radicand};
    }

    void addTerm(const FractionValue& coefficient,
                 const Integer& degree, const Integer& radicand)
    {
        if (coefficient == FractionValue(0)) return;
        const auto simplified = simplifyRadical(degree, radicand);
        const FractionValue normalized = coefficient * FractionValue(simplified.first);
        if (simplified.second == 1) { rationalPart += normalized; return; }
        for (auto& term : radicalTerms)
        {
            if (term.degree == degree && term.radicand == simplified.second)
            {
                term.coefficient += normalized;
                removeZeroTerms();
                return;
            }
        }
        radicalTerms.push_back({normalized, degree, simplified.second});
    }

    void removeZeroTerms()
    {
        for (auto it = radicalTerms.begin(); it != radicalTerms.end(); )
        {
            if (it->coefficient == FractionValue(0)) it = radicalTerms.erase(it);
            else ++it;
        }
    }

    static Integer smallestPrimeFactor(const Integer& n)
    {
        for (Integer p = 2; p <= n / p; ++p) if (n % p == 0) return p;
        return n;
    }

public:
    IrrationalNumber() = default;
    IrrationalNumber(long long value) : rationalPart(Integer(value)) {}
    IrrationalNumber(const Integer& value) : rationalPart(value) {}
    IrrationalNumber(const FractionValue& value) : rationalPart(value) {}

    template <FractionType T>
    IrrationalNumber(const Fraction<T>& value) : rationalPart(value) {}

    template <FractionType T>
    IrrationalNumber(const Fraction<T>& coefficient,
                     const Integer& degree, const Integer& radicand)
    {
        addTerm(FractionValue(coefficient), degree, radicand);
    }

    IrrationalNumber(const Integer& degree, const Integer& radicand)
    { addTerm(FractionValue(1), degree, radicand); }

    template <FractionType T, typename R>
    IrrationalNumber(const Fraction<T>& rational,
                     const std::vector<std::pair<Fraction<T>, R>>& terms)
        : rationalPart(rational)
    {
        for (const auto& term : terms)
            addTerm(FractionValue(term.first), Integer(2), Integer(term.second));
    }

    double aprox() const override
    {
        double result = rationalPart.aprox();
        for (const auto& term : radicalTerms)
            result += term.coefficient.aprox() * std::pow(
                term.radicand.convert_to<double>(),
                1.0 / term.degree.convert_to<double>());
        return result;
    }

    std::string toString() const override
    { std::ostringstream out; out << *this; return out.str(); }

    bool isRational() const { return radicalTerms.empty(); }
    const FractionValue& getRationalPart() const { return rationalPart; }
    const std::vector<RadicalTerm>& getRadicalTerms() const { return radicalTerms; }
    struct ApproxTerm { FractionValue coefficient; Integer degree; Integer radicand; };
    std::vector<ApproxTerm> approximationTerms() const
    { std::vector<ApproxTerm> out; for(const auto& t:radicalTerms) out.push_back({t.coefficient,t.degree,t.radicand}); return out; }
    Number::Decimal aprox100() const override
    {
        using D=Number::Decimal;
        auto frac=[](const FractionValue& f){ return D(f.getNumerator().convert_to<std::string>())/D(f.getDenominator().convert_to<std::string>()); };
        D result=frac(rationalPart);
        for(const auto& t:radicalTerms)
            result += frac(t.coefficient)*boost::multiprecision::pow(D(t.radicand.convert_to<std::string>()),D(1)/D(t.degree.convert_to<std::string>()));
        return result;
    }

    IrrationalNumber operator+(const IrrationalNumber& other) const
    {
        IrrationalNumber result = *this;
        result.rationalPart += other.rationalPart;
        for (const auto& t : other.radicalTerms) result.addTerm(t.coefficient, t.degree, t.radicand);
        return result;
    }

    IrrationalNumber operator-(const IrrationalNumber& other) const
    {
        IrrationalNumber result = *this;
        result.rationalPart -= other.rationalPart;
        for (const auto& t : other.radicalTerms) result.addTerm(FractionValue(0) - t.coefficient, t.degree, t.radicand);
        return result;
    }

    IrrationalNumber operator*(const IrrationalNumber& other) const
    {
        IrrationalNumber result;
        result.rationalPart = rationalPart * other.rationalPart;
        for (const auto& t : radicalTerms) result.addTerm(t.coefficient * other.rationalPart, t.degree, t.radicand);
        for (const auto& t : other.radicalTerms) result.addTerm(t.coefficient * rationalPart, t.degree, t.radicand);
        for (const auto& a : radicalTerms) for (const auto& b : other.radicalTerms)
        {
            const Integer common = lcm(a.degree, b.degree);
            const Integer product = integerPower(a.radicand, common / a.degree)
                                  * integerPower(b.radicand, common / b.degree);
            result.addTerm(a.coefficient * b.coefficient, common, product);
        }
        result.removeZeroTerms();
        return result;
    }

    IrrationalNumber operator^(int exponent) const
    {
        if (exponent == 0) return IrrationalNumber(1);
        const bool negative = exponent < 0;
        unsigned long long power = negative
            ? static_cast<unsigned long long>(-(static_cast<long long>(exponent)))
            : static_cast<unsigned long long>(exponent);
        IrrationalNumber result(1), base = *this;
        while (power)
        {
            if (power & 1ULL) result = result * base;
            power >>= 1;
            if (power) base = base * base;
        }
        return negative ? IrrationalNumber(1) / result : result;
    }

    IrrationalNumber operator/(const FractionValue& divisor) const
    {
        if (divisor == FractionValue(0)) throw std::invalid_argument("Division by zero.");
        IrrationalNumber result;
        result.rationalPart = rationalPart / divisor;
        for (const auto& t : radicalTerms) result.addTerm(t.coefficient / divisor, t.degree, t.radicand);
        return result;
    }

    IrrationalNumber operator/(long long divisor) const
    { return *this / FractionValue(Integer(divisor)); }

    IrrationalNumber operator/(const IrrationalNumber& divisor) const
    {
        if (divisor.rationalPart == FractionValue(0) && divisor.radicalTerms.empty())
            throw std::invalid_argument("Division by zero.");
        if (divisor.radicalTerms.empty()) return *this / divisor.rationalPart;

        if (divisor.radicalTerms.size() == 1 && divisor.rationalPart == FractionValue(0))
        {
            const auto& t = divisor.radicalTerms.front();
            const Integer numeratorRadicand = integerPower(t.radicand, t.degree - 1);
            IrrationalNumber numerator = *this * IrrationalNumber(t.degree, numeratorRadicand);
            return numerator / (t.coefficient * FractionValue(t.radicand));
        }

        if (divisor.radicalTerms.size() == 1)
        {
            const auto& t = divisor.radicalTerms.front();
            IrrationalNumber conjugate(0);
            for (Integer k = 0; k < t.degree; ++k)
            {
                FractionValue coefficient =
                    fractionPower(divisor.rationalPart, t.degree - 1 - k)
                    * fractionPower(t.coefficient, k);
                if ((k & 1) != 0) coefficient = FractionValue(0) - coefficient;
                if (k == 0) conjugate.rationalPart += coefficient;
                else conjugate.addTerm(coefficient, t.degree,
                                       integerPower(t.radicand, k));
            }
            const IrrationalNumber norm = divisor * conjugate;
            if (!norm.radicalTerms.empty())
                throw std::runtime_error("Failed to rationalize single-radical denominator.");
            return (*this * conjugate) / norm.rationalPart;
        }

        if (divisor.rationalPart == FractionValue(0)
            && divisor.radicalTerms.size() == 2
            && divisor.radicalTerms[0].degree == divisor.radicalTerms[1].degree)
        {
            const auto& a = divisor.radicalTerms[0];
            const auto& b = divisor.radicalTerms[1];
            IrrationalNumber conjugate(0);
            for (Integer k = 0; k < a.degree; ++k)
            {
                const Integer aPower = a.degree - 1 - k;
                const Integer bPower = k;
                FractionValue coefficient = fractionPower(a.coefficient, aPower)
                                          * fractionPower(b.coefficient, bPower);
                if ((k & 1) != 0) coefficient = FractionValue(0) - coefficient;
                conjugate.addTerm(coefficient, a.degree,
                    integerPower(a.radicand, aPower) * integerPower(b.radicand, bPower));
            }
            const IrrationalNumber norm = divisor * conjugate;
            if (!norm.radicalTerms.empty())
                throw std::runtime_error("Failed to rationalize two-root denominator.");
            return (*this * conjugate) / norm.rationalPart;
        }

        for (const auto& t : divisor.radicalTerms)
            if (t.degree != 2) throw std::runtime_error("Multi-term non-square radical rationalization is not implemented.");

        const Integer p = smallestPrimeFactor(divisor.radicalTerms.front().radicand);
        IrrationalNumber conjugate = divisor;
        for (auto& term : conjugate.radicalTerms)
            if (term.radicand % p == 0) term.coefficient = FractionValue(0) - term.coefficient;
        IrrationalNumber norm = divisor * conjugate;
        for (const auto& term : norm.radicalTerms)
            if (term.radicand % p == 0) throw std::runtime_error("Failed to rationalize denominator.");
        return (*this * conjugate) / norm;
    }

    IrrationalNumber& operator+=(const IrrationalNumber& o) { return *this = *this + o; }
    IrrationalNumber& operator-=(const IrrationalNumber& o) { return *this = *this - o; }
    IrrationalNumber& operator*=(const IrrationalNumber& o) { return *this = *this * o; }
    IrrationalNumber& operator/=(const IrrationalNumber& o) { return *this = *this / o; }
    IrrationalNumber& operator/=(const FractionValue& o) { return *this = *this / o; }
    IrrationalNumber& operator/=(long long o) { return *this = *this / o; }

    bool operator==(const IrrationalNumber& other) const
    {
        if (rationalPart != other.rationalPart || radicalTerms.size() != other.radicalTerms.size()) return false;
        for (const auto& t : radicalTerms)
        {
            bool found = false;
            for (const auto& u : other.radicalTerms)
                if (t.degree == u.degree && t.radicand == u.radicand && t.coefficient == u.coefficient) { found = true; break; }
            if (!found) return false;
        }
        return true;
    }
    bool operator!=(const IrrationalNumber& other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream& out, const IrrationalNumber& n)
    {
        bool wrote = false;
        if (n.rationalPart != FractionValue(0) || n.radicalTerms.empty()) { out << n.rationalPart; wrote = true; }
        for (const auto& t : n.radicalTerms)
        {
            std::ostringstream cs; cs << t.coefficient;
            std::string c = cs.str();
            const bool neg = !c.empty() && c[0] == '-';
            if (wrote) out << (neg ? " - " : " + "); else if (neg) out << '-';
            if (neg) c.erase(0, 1);
            if (c != "1") out << c;
            if (t.degree == 2) out << "sqrt(" << t.radicand << ')';
            else out << "root(" << t.degree << ',' << t.radicand << ')';
            wrote = true;
        }
        return out;
    }
};
