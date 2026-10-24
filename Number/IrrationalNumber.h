
#pragma once

#include "RealNumber.h"
#include "Fraction.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <utility>

class IrrationalNumber : public RealNumber
{
private:
    struct RadicalTerm
    {
        Fraction<long long> coefficient;
        long long radicand;

        RadicalTerm(
            const Fraction<long long>& coefficient,
            long long radicand
        )
            : coefficient(coefficient),
              radicand(radicand)
        {
        }
    };

    // 유리수 부분
    Fraction<long long> rationalPart;

    // coefficient * sqrt(radicand) 항들
    std::vector<RadicalTerm> radicalTerms;

    // n = coefficient^2 * radicand 형태로 단순화
    static std::pair<long long, long long>
    simplifyRadical(long long n)
    {
        if (n <= 0)
        {
            throw std::invalid_argument(
                "Radicand must be positive."
            );
        }

        long long coefficient = 1;
        long long radicand = 1;

        for (long long p = 2; p <= n / p; ++p)
        {
            int exponent = 0;

            while (n % p == 0)
            {
                n /= p;
                ++exponent;
            }

            for (int i = 0; i < exponent / 2; ++i)
            {
                if (coefficient >
                    std::numeric_limits<long long>::max() / p)
                {
                    throw std::overflow_error(
                        "Radical coefficient overflow."
                    );
                }

                coefficient *= p;
            }

            if (exponent % 2 == 1)
            {
                if (radicand >
                    std::numeric_limits<long long>::max() / p)
                {
                    throw std::overflow_error(
                        "Radicand overflow."
                    );
                }

                radicand *= p;
            }
        }

        if (n > 1)
        {
            if (radicand >
                std::numeric_limits<long long>::max() / n)
            {
                throw std::overflow_error(
                    "Radicand overflow."
                );
            }

            radicand *= n;
        }

        return { coefficient, radicand };
    }

    // 근호 항을 추가하고 동류항을 정리
    void addTerm(
        const Fraction<long long>& coefficient,
        long long radicand
    )
    {
        if (coefficient == Fraction<long long>(0))
            return;

        auto simplified = simplifyRadical(radicand);

        long long factor = simplified.first;
        long long newRadicand = simplified.second;

        Fraction<long long> newCoefficient =
            coefficient * Fraction<long long>(factor);

        // sqrt(1) = 1이므로 유리수 부분으로 이동
        if (newRadicand == 1)
        {
            rationalPart += newCoefficient;
            return;
        }

        // 같은 근호를 가진 항끼리 합치기
        for (auto& term : radicalTerms)
        {
            if (term.radicand == newRadicand)
            {
                term.coefficient += newCoefficient;
                return;
            }
        }

        radicalTerms.emplace_back(
            newCoefficient,
            newRadicand
        );
    }

    void removeZeroTerms()
    {
        for (auto it = radicalTerms.begin();
             it != radicalTerms.end();)
        {
            if (it->coefficient == Fraction<long long>(0))
                it = radicalTerms.erase(it);
            else
                ++it;
        }
    }

public:
    IrrationalNumber()
        : rationalPart(0)
    {
    }

    // 정수
    IrrationalNumber(long long value)
        : rationalPart(value)
    {
    }

    // sqrt(n)
    explicit IrrationalNumber(long long radicand, bool asRadical)
        : rationalPart(0)
    {
        if (asRadical)
            addTerm(Fraction<long long>(1), radicand);
        else
            rationalPart = Fraction<long long>(radicand);
    }

    // coefficient * sqrt(radicand)
    IrrationalNumber(
        const Fraction<long long>& coefficient,
        long long radicand
    )
        : rationalPart(0)
    {
        addTerm(coefficient, radicand);
    }

    // 유리수 부분과 근호 항 목록을 복사
    IrrationalNumber(
        const Fraction<long long>& rational,
        const std::vector<std::pair<
            Fraction<long long>,
            long long
        >>& terms
    )
        : rationalPart(rational)
    {
        for (const auto& term : terms)
            addTerm(term.first, term.second);
    }

    double aprox() const override
    {
        double result = rationalPart.aprox();

        for (const auto& term : radicalTerms)
        {
            result += term.coefficient.aprox()
                    * std::sqrt(
                        static_cast<double>(term.radicand)
                    );
        }

        return result;
    }

    std::string toString() const override
    {
        std::ostringstream oss;
        oss << *this;
        return oss.str();
    }

    // 덧셈
    IrrationalNumber operator+(
        const IrrationalNumber& other
    ) const
    {
        IrrationalNumber result = *this;

        result.rationalPart += other.rationalPart;

        for (const auto& term : other.radicalTerms)
        {
            result.addTerm(
                term.coefficient,
                term.radicand
            );
        }

        result.removeZeroTerms();
        return result;
    }

    // 뺄셈
    IrrationalNumber operator-(
        const IrrationalNumber& other
    ) const
    {
        IrrationalNumber result = *this;

        result.rationalPart -= other.rationalPart;

        for (const auto& term : other.radicalTerms)
        {
            result.addTerm(
                Fraction<long long>(0) - term.coefficient,
                term.radicand
            );
        }

        result.removeZeroTerms();
        return result;
    }

    // 곱셈
    IrrationalNumber operator*(
        const IrrationalNumber& other
    ) const
    {
        IrrationalNumber result;

        // 유리수 * 유리수
        result.rationalPart =
            rationalPart * other.rationalPart;

        // 첫 번째 유리수 부분 * 두 번째 근호 항들
        for (const auto& term : other.radicalTerms)
        {
            result.addTerm(
                rationalPart * term.coefficient,
                term.radicand
            );
        }

        // 두 번째 유리수 부분 * 첫 번째 근호 항들
        for (const auto& term : radicalTerms)
        {
            result.addTerm(
                other.rationalPart * term.coefficient,
                term.radicand
            );
        }

        // 근호 항 * 근호 항
        for (const auto& a : radicalTerms)
        {
            for (const auto& b : other.radicalTerms)
            {
                if (a.radicand >
                    std::numeric_limits<long long>::max()
                    / b.radicand)
                {
                    throw std::overflow_error(
                        "Radicand multiplication overflow."
                    );
                }

                long long product =
                    a.radicand * b.radicand;

                result.addTerm(
                    a.coefficient * b.coefficient,
                    product
                );
            }
        }

        result.removeZeroTerms();
        return result;
    }

    // 유리수로 나누기
    IrrationalNumber operator/(
        const Fraction<long long>& divisor
    ) const
    {
        if (divisor == Fraction<long long>(0))
        {
            throw std::invalid_argument(
                "Division by zero."
            );
        }

        IrrationalNumber result;

        result.rationalPart = rationalPart / divisor;

        for (const auto& term : radicalTerms)
        {
            result.addTerm(
                term.coefficient / divisor,
                term.radicand
            );
        }

        result.removeZeroTerms();
        return result;
    }

    // 복합 대입 연산자
    IrrationalNumber& operator+=(
        const IrrationalNumber& other
    )
    {
        *this = *this + other;
        return *this;
    }

    IrrationalNumber& operator-=(
        const IrrationalNumber& other
    )
    {
        *this = *this - other;
        return *this;
    }

    IrrationalNumber& operator*=(
        const IrrationalNumber& other
    )
    {
        *this = *this * other;
        return *this;
    }

    IrrationalNumber& operator/=(
        const Fraction<long long>& divisor
    )
    {
        *this = *this / divisor;
        return *this;
    }

    friend std::ostream& operator<<(
        std::ostream& os,
        const IrrationalNumber& number
    )
    {
        bool hasOutput = false;

        if (!(number.rationalPart == Fraction<long long>(0))
            || number.radicalTerms.empty())
        {
            os << number.rationalPart;
            hasOutput = true;
        }

        for (const auto& term : number.radicalTerms)
        {
            if (hasOutput)
                os << " + ";

            os << term.coefficient
               << "sqrt(" << term.radicand << ")";

            hasOutput = true;
        }

        return os;
    }
};
