
#pragma once

#include "RealNumber.h"
#include "Fraction.h"

#include <sstream>
#include <string>

class RationalNumber : public RealNumber
{
private:
    Fraction<long long> value;

public:
    RationalNumber()
        : value(0)
    {
    }

    RationalNumber(long long numerator)
        : value(numerator)
    {
    }

    RationalNumber(long long numerator, long long denominator)
        : value(numerator, denominator)
    {
    }

    RationalNumber(const Fraction<long long>& fraction)
        : value(fraction)
    {
    }

    double aprox() const override
    {
        return value.aprox();
    }

    std::string toString() const override
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    const Fraction<long long>& getFraction() const
    {
        return value;
    }

    // ����
    RationalNumber operator+(const RationalNumber& other) const
    {
        return RationalNumber(value + other.value);
    }

    // ����
    RationalNumber operator-(const RationalNumber& other) const
    {
        return RationalNumber(value - other.value);
    }

    // ����
    RationalNumber operator*(const RationalNumber& other) const
    {
        return RationalNumber(value * other.value);
    }

    // ������
    RationalNumber operator/(const RationalNumber& other) const
    {
        return RationalNumber(value / other.value);
    }

    // ���� ����
    RationalNumber& operator+=(const RationalNumber& other)
    {
        value += other.value;
        return *this;
    }

    RationalNumber& operator-=(const RationalNumber& other)
    {
        value -= other.value;
        return *this;
    }

    RationalNumber& operator*=(const RationalNumber& other)
    {
        value *= other.value;
        return *this;
    }

    RationalNumber& operator/=(const RationalNumber& other)
    {
        value /= other.value;
        return *this;
    }

    // �ŵ�����
    RationalNumber operator^(int exponent) const
    {
        return RationalNumber(value ^ exponent);
    }

    RationalNumber& operator^=(int exponent)
    {
        value ^= exponent;
        return *this;
    }

    friend std::ostream& operator<<(
        std::ostream& os,
        const RationalNumber& number)
    {
        os << number.value;
        return os;
    }
};
