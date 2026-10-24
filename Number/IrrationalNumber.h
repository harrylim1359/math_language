
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

    IrrationalNumber operator^(int exponent) const
    {
        // 0의 음수 지수는 정의되지 않음
        if (exponent < 0
            && rationalPart == Fraction<long long>(0)
            && radicalTerms.empty())
        {
            throw std::invalid_argument(
                "Zero cannot have a negative exponent."
            );
        }

        // 음수 지수: a^(-n) = 1 / a^n
        if (exponent < 0)
        {
            // -INT_MIN의 오버플로 방지
            long long positiveExponent =
                -static_cast<long long>(exponent);

            IrrationalNumber result = (*this) ^ (
                static_cast<int>(positiveExponent)
            );

            return IrrationalNumber(1) / result;
        }

        // 기존의 0 이상 정수 지수 계산
        IrrationalNumber result(1);
        IrrationalNumber base = *this;
        int power = exponent;

        // 분할 정복 방식의 거듭제곱
        while (power > 0)
        {
            if (power % 2 == 1)
            {
                result *= base;
            }

            power /= 2;

            if (power > 0)
            {
                base *= base;
            }
        }

        return result;
    }


    IrrationalNumber& operator^=(int exponent)
    {
        *this = *this ^ exponent;
        return *this;
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


    // 무리수식끼리의 나눗셈 및 다중 켤레식을 이용한 유리화
    IrrationalNumber operator/(const IrrationalNumber& divisor) const
    {
        // 분모가 0인지 확인
        if (divisor.rationalPart == Fraction<long long>(0)
            && divisor.radicalTerms.empty())
        {
            throw std::invalid_argument("Division by zero.");
        }

        // 분모가 유리수뿐이면 기존 나눗셈을 사용
        if (divisor.radicalTerms.empty())
        {
            return *this / divisor.rationalPart;
        }

        /*
            1. 분모에 포함된 제곱근의 소인수 p를 선택한다.
            2. p를 포함하는 모든 제곱근 항의 부호를 바꿔 켤레식을 만든다.
            3. 원래 분모와 켤레식을 곱한다.
            4. 새 분모에는 p가 포함된 제곱근 항이 사라진다.
            5. 남은 분모에 대해 같은 과정을 반복한다.
        */

        // 첫 번째 제곱근 항에서 소인수 하나 찾기
        long long n = divisor.radicalTerms[0].radicand;
        long long p = n;

        for (long long candidate = 2;
             candidate <= n / candidate;
             ++candidate)
        {
            if (n % candidate == 0)
            {
                p = candidate;
                break;
            }
        }

        // 켤레식 생성
        IrrationalNumber conjugate = divisor;

        for (auto& term : conjugate.radicalTerms)
        {
            // sqrt(p)가 포함된 항의 부호를 반전
            if (term.radicand % p == 0)
            {
                term.coefficient =
                    Fraction<long long>(0) - term.coefficient;
            }
        }

        // 분모와 켤레식의 곱
        IrrationalNumber norm = divisor * conjugate;

        // 분모가 0이 되는 경우 방지
        if (norm.rationalPart == Fraction<long long>(0)
            && norm.radicalTerms.empty())
        {
            throw std::invalid_argument("Division by zero.");
        }

        // 선택한 소인수가 포함된 항이 제거되었는지 확인
        for (const auto& term : norm.radicalTerms)
        {
            if (term.radicand % p == 0)
            {
                throw std::runtime_error(
                    "Failed to rationalize the denominator."
                );
            }
        }

        // 분자에도 켤레식을 곱하고,
        // 새 분모에 대해 재귀적으로 유리화
        IrrationalNumber numerator = *this * conjugate;

        return numerator / norm;
    }

    // 정수로 나누기: 정수 리터럴의 오버로드 모호성 방지
    IrrationalNumber operator/(long long divisor) const
    {
        return *this / Fraction<long long>(divisor);
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

    IrrationalNumber& operator/=(const IrrationalNumber& divisor)
    {
        *this = *this / divisor;
        return *this;
    }

    IrrationalNumber& operator/=(long long divisor)
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
            std::ostringstream coefficientStream;
            coefficientStream << term.coefficient;
            std::string coefficientText = coefficientStream.str();

            bool negative = !coefficientText.empty()
                         && coefficientText[0] == '-';

            if (hasOutput)
                os << (negative ? " - " : " + ");
            else if (negative)
                os << "-";

            if (negative)
                coefficientText.erase(0, 1);

            // 계수의 절댓값이 1이면 생략
            if (coefficientText != "1")
                os << coefficientText;

            os << "sqrt(" << term.radicand << ")";
            hasOutput = true;
        }

        return os;
    }
};
