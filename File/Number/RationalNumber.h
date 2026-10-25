#pragma once

#include "RealNumber.h"
#include "Fraction.h"
#include "BigInteger.h"

#include <sstream>
#include <string>

class RationalNumber : public RealNumber
{
private:
    Fraction<BigInteger> value;

public:
    RationalNumber() : value(0) {}
    RationalNumber(long long numerator) : value(BigInteger(numerator)) {}
    RationalNumber(const BigInteger& numerator) : value(numerator) {}
    RationalNumber(const BigInteger& numerator, const BigInteger& denominator)
        : value(numerator, denominator) {}

    template <FractionType T>
    RationalNumber(const Fraction<T>& fraction) : value(fraction) {}

    double aprox() const override { return value.aprox(); }
    Number::Decimal aprox100() const override
    {
        return Number::Decimal(value.getNumerator().convert_to<std::string>())
             / Number::Decimal(value.getDenominator().convert_to<std::string>());
    }

    std::string toString() const override
    {
        std::ostringstream out;
        out << value;
        return out.str();
    }

    const Fraction<BigInteger>& getFraction() const { return value; }

    RationalNumber operator+(const RationalNumber& other) const
    { return RationalNumber(value + other.value); }
    RationalNumber operator-(const RationalNumber& other) const
    { return RationalNumber(value - other.value); }
    RationalNumber operator*(const RationalNumber& other) const
    { return RationalNumber(value * other.value); }
    RationalNumber operator/(const RationalNumber& other) const
    { return RationalNumber(value / other.value); }
    RationalNumber operator^(int exponent) const
    { return RationalNumber(value ^ exponent); }

    RationalNumber& operator+=(const RationalNumber& other)
    { value += other.value; return *this; }
    RationalNumber& operator-=(const RationalNumber& other)
    { value -= other.value; return *this; }
    RationalNumber& operator*=(const RationalNumber& other)
    { value *= other.value; return *this; }
    RationalNumber& operator/=(const RationalNumber& other)
    { value /= other.value; return *this; }
    RationalNumber& operator^=(int exponent)
    { value ^= exponent; return *this; }

    friend std::ostream& operator<<(std::ostream& out, const RationalNumber& n)
    { return out << n.value; }
};
