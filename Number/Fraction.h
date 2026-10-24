#pragma once

#include <iostream>
#include <cmath>
#include <stdexcept>
#include <type_traits>
#include <concepts>

template <typename T>
T power(T base, int exponent)
{
    if (exponent < 0)
        throw std::invalid_argument(
            "Negative exponent requires a fractional result."
        );

    T result = T(1);

    while (exponent > 0)
    {
        if (exponent % 2 == 1)
            result *= base;

        base *= base;
        exponent /= 2;
    }

    return result;
}

// T가 +, -, *, /, % 연산을 지원하는지 확인
template <typename T>
concept FractionType =
    requires(T a, T b)
    {
        { a + b } -> std::convertible_to<T>;
        { a - b } -> std::convertible_to<T>;
        { a * b } -> std::convertible_to<T>;
        { a / b } -> std::convertible_to<T>;
        { a % b } -> std::convertible_to<T>;
    };


template <FractionType T>
class Fraction
{
private:
    T numerator;
    T denominator;

    void simplify()
    {
        if (denominator == T(0))
            throw std::invalid_argument("Fraction denominator cannot be zero.");

        // 분모를 양수로 통일
        if (denominator < T(0))
        {
            numerator = -numerator;
            denominator = -denominator;
        }

        T g = gcd(numerator, denominator);

        numerator /= g;
        denominator /= g;
    }

    static T gcd(T a, T b)
    {
        if (a < T(0))
            a = -a;

        if (b < T(0))
            b = -b;

        while (b != T(0))
        {
            T temp = a % b;
            a = b;
            b = temp;
        }

        return a;
    }

public:

    // 기본 생성자
    Fraction()
        : numerator(0), denominator(1)
    {
    }

    // Fraction(a)
    Fraction(T value)
        : numerator(value), denominator(1)
    {
    }

    // Fraction(a, b)
    Fraction(T num, T den)
        : numerator(num), denominator(den)
    {
        simplify();
    }

    // 분자
    T getNumerator() const
    {
        return numerator;
    }

    // 분모
    T getDenominator() const
    {
        return denominator;
    }

    // 근삿값
    double aprox() const
    {
        return static_cast<double>(numerator)
             / static_cast<double>(denominator);
    }


    // =========================
    // +
    // =========================

    Fraction operator+(const Fraction& other) const
    {
        return Fraction(
            numerator * other.denominator
            + other.numerator * denominator,

            denominator * other.denominator
        );
    }


    // =========================
    // -
    // =========================

    Fraction operator-(const Fraction& other) const
    {
        return Fraction(
            numerator * other.denominator
            - other.numerator * denominator,

            denominator * other.denominator
        );
    }


    // =========================
    // *
    // =========================

    Fraction operator*(const Fraction& other) const
    {
        return Fraction(
            numerator * other.numerator,
            denominator * other.denominator
        );
    }


    // =========================
    // /
    // =========================

    Fraction operator/(const Fraction& other) const
    {
        if (other.numerator == T(0))
            throw std::invalid_argument("Division by zero.");

        return Fraction(
            numerator * other.denominator,
            denominator * other.numerator
        );
    }


    // =========================
    // 비교
    // =========================

    bool operator==(const Fraction& other) const
    {
        return numerator == other.numerator
            && denominator == other.denominator;
    }

    bool operator!=(const Fraction& other) const
    {
        return !(*this == other);
    }

    // =========================
    // +=
    // =========================

    Fraction& operator+=(const Fraction& other)
    {
        numerator =
            numerator * other.denominator
            + other.numerator * denominator;

        denominator =
            denominator * other.denominator;

        simplify();

        return *this;
    }


    // =========================
    // -=
    // =========================

    Fraction& operator-=(const Fraction& other)
    {
        numerator =
            numerator * other.denominator
            - other.numerator * denominator;

        denominator =
            denominator * other.denominator;

        simplify();

        return *this;
    }


    // =========================
    // *=
    // =========================

    Fraction& operator*=(const Fraction& other)
    {
        numerator *= other.numerator;
        denominator *= other.denominator;

        simplify();

        return *this;
    }


    // =========================
    // /=
    // =========================

    Fraction& operator/=(const Fraction& other)
    {
        if (other.numerator == T(0))
            throw std::invalid_argument("Division by zero.");

        numerator *= other.denominator;
        denominator *= other.numerator;

        simplify();

        return *this;
    }


    // =========================
    // ^
    // =========================

    Fraction operator^(int exponent) const
    {
        if (exponent == 0)
            return Fraction(T(1));

        if (exponent < 0)
        {
            return Fraction(
                power(denominator, -exponent),
                power(numerator, -exponent)
            );
        }

        return Fraction(
            power(numerator, exponent),
            power(denominator, exponent)
        );
    }


    // =========================
    // ^=
    // =========================

    Fraction& operator^=(int exponent)
    {
        *this = *this ^ exponent;

        return *this;
    }


    // =========================
    // 출력
    // =========================

    friend std::ostream& operator<<(
        std::ostream& os,
        const Fraction& f)
    {
        if (f.denominator == T(1))
        {
            os << f.numerator;
        }
        else
        {
            os << f.numerator << "/" << f.denominator;
        }

        return os;
    }
};
