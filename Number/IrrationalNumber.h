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

    // =========================================================
    // 근호 항
    //
    // coefficient * root(degree, radicand)
    //
    // 예:
    // 2sqrt(3)
    // -> coefficient = 2
    // -> degree = 2
    // -> radicand = 3
    //
    // 3cbrt(5)
    // -> coefficient = 3
    // -> degree = 3
    // -> radicand = 5
    // =========================================================

    struct RadicalTerm
    {
        Fraction<long long> coefficient;

        long long degree;
        long long radicand;

        RadicalTerm(
            const Fraction<long long>& coefficient,
            long long degree,
            long long radicand
        )
            : coefficient(coefficient),
              degree(degree),
              radicand(radicand)
        {
        }
    };


    // =========================================================
    // 유리수 부분
    // =========================================================

    Fraction<long long> rationalPart;


    // =========================================================
    // 근호 항 목록
    // =========================================================

    std::vector<RadicalTerm> radicalTerms;


    // =========================================================
    // 최대공약수
    // =========================================================

    static long long gcd(
        long long a,
        long long b
    )
    {
        if (a < 0)
            a = -a;

        if (b < 0)
            b = -b;

        while (b != 0)
        {
            long long temp = a % b;
            a = b;
            b = temp;
        }

        return a;
    }


    // =========================================================
    // 최소공배수
    // =========================================================

    static long long lcm(
        long long a,
        long long b
    )
    {
        long long g = gcd(a, b);

        if (g == 0)
            return 0;

        if (a / g >
            std::numeric_limits<long long>::max() / b)
        {
            throw std::overflow_error(
                "LCM overflow."
            );
        }

        return (a / g) * b;
    }


    // =========================================================
    // 근호 단순화
    //
    // root(degree, n)
    //
    // 을
    //
    // coefficient * root(degree, radicand)
    //
    // 형태로 변환
    //
    // 예:
    //
    // sqrt(8)
    // -> 2sqrt(2)
    //
    // cbrt(16)
    // -> 2cbrt(2)
    //
    // root(4, 16)
    // -> 2
    // =========================================================

    static std::pair<long long, long long>
    simplifyRadical(
        long long degree,
        long long n
    )
    {
        if (degree < 2)
        {
            throw std::invalid_argument(
                "Root degree must be at least 2."
            );
        }

        if (n <= 0)
        {
            throw std::invalid_argument(
                "Radicand must be positive."
            );
        }


        long long coefficient = 1;
        long long radicand = 1;


        // =====================================================
        // 소인수분해
        // =====================================================

        for (
            long long p = 2;
            p <= n / p;
            ++p
        )
        {
            int exponent = 0;

            while (n % p == 0)
            {
                n /= p;
                ++exponent;
            }


            // -------------------------------------------------
            // degree개가 모이면 근호 밖으로 빠짐
            // -------------------------------------------------

            for (
                long long i = 0;
                i < exponent / degree;
                ++i
            )
            {
                if (
                    coefficient >
                    std::numeric_limits<long long>::max() / p
                )
                {
                    throw std::overflow_error(
                        "Radical coefficient overflow."
                    );
                }

                coefficient *= p;
            }


            // -------------------------------------------------
            // 나머지는 근호 안에 남음
            // -------------------------------------------------

            long long remainder =
                exponent % degree;

            for (
                long long i = 0;
                i < remainder;
                ++i
            )
            {
                if (
                    radicand >
                    std::numeric_limits<long long>::max() / p
                )
                {
                    throw std::overflow_error(
                        "Radicand overflow."
                    );
                }

                radicand *= p;
            }
        }


        // =====================================================
        // 마지막 소인수
        // =====================================================

        if (n > 1)
        {
            if (
                radicand >
                std::numeric_limits<long long>::max() / n
            )
            {
                throw std::overflow_error(
                    "Radicand overflow."
                );
            }

            radicand *= n;
        }


        return {
            coefficient,
            radicand
        };
    }


    // =========================================================
    // 근호 항 추가
    //
    // 동류항 정리까지 수행
    // =========================================================

    void addTerm(
        const Fraction<long long>& coefficient,
        long long degree,
        long long radicand
    )
    {
        if (
            coefficient ==
            Fraction<long long>(0)
        )
        {
            return;
        }


        // -----------------------------------------------------
        // 근호 단순화
        // -----------------------------------------------------

        auto simplified =
            simplifyRadical(
                degree,
                radicand
            );

        long long factor =
            simplified.first;

        long long newRadicand =
            simplified.second;


        Fraction<long long> newCoefficient =
            coefficient *
            Fraction<long long>(factor);


        // -----------------------------------------------------
        // root(degree, 1) = 1
        // -----------------------------------------------------

        if (newRadicand == 1)
        {
            rationalPart += newCoefficient;
            return;
        }


        // -----------------------------------------------------
        // 같은 차수 + 같은 radicand이면 동류항
        // -----------------------------------------------------

        for (auto& term : radicalTerms)
        {
            if (
                term.degree == degree &&
                term.radicand == newRadicand
            )
            {
                term.coefficient +=
                    newCoefficient;

                return;
            }
        }


        // -----------------------------------------------------
        // 새로운 항 추가
        // -----------------------------------------------------

        radicalTerms.emplace_back(
            newCoefficient,
            degree,
            newRadicand
        );
    }


    // =========================================================
    // 계수가 0인 근호 항 제거
    // =========================================================

    void removeZeroTerms()
    {
        for (
            auto it = radicalTerms.begin();
            it != radicalTerms.end();
        )
        {
            if (
                it->coefficient ==
                Fraction<long long>(0)
            )
            {
                it = radicalTerms.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }


public:

    // =========================================================
    // 기본 생성자
    // =========================================================

    IrrationalNumber()
        : rationalPart(0)
    {
    }


    // =========================================================
    // 정수
    // =========================================================

    IrrationalNumber(long long value)
        : rationalPart(value)
    {
    }


    // =========================================================
    // coefficient * root(degree, radicand)
    //
    // 예:
    //
    // IrrationalNumber(
    //     Fraction<long long>(2),
    //     3,
    //     5
    // )
    //
    // -> 2cbrt(5)
    // =========================================================

    IrrationalNumber(
        const Fraction<long long>& coefficient,
        long long degree,
        long long radicand
    )
        : rationalPart(0)
    {
        addTerm(
            coefficient,
            degree,
            radicand
        );
    }


    // =========================================================
    // root(degree, radicand)
    //
    // 예:
    //
    // IrrationalNumber(3, 2)
    //
    // -> cbrt(2)
    //
    // 주의:
    // sqrt(2)는 기존 생성자인
    // IrrationalNumber(2, true)를 사용
    // =========================================================

    IrrationalNumber(
        long long degree,
        long long radicand
    )
        : rationalPart(0)
    {
        addTerm(
            Fraction<long long>(1),
            degree,
            radicand
        );
    }


    // =========================================================
    // 기존 형식의 생성자
    //
    // rational + coefficient * sqrt(radicand)
    //
    // 기존 테스트와의 호환성을 유지
    // =========================================================

    IrrationalNumber(
        const Fraction<long long>& rational,
        const std::vector<
            std::pair<
                Fraction<long long>,
                long long
            >
        >& terms
    )
        : rationalPart(rational)
    {
        for (const auto& term : terms)
        {
            addTerm(
                term.first,
                2,
                term.second
            );
        }
    }


    // =========================================================
    // 근삿값
    // =========================================================

    double aprox() const override
    {
        double result =
            rationalPart.aprox();


        for (const auto& term : radicalTerms)
        {
            result +=
                term.coefficient.aprox()
                *
                std::pow(
                    static_cast<double>(
                        term.radicand
                    ),
                    1.0 /
                    static_cast<double>(
                        term.degree
                    )
                );
        }


        return result;
    }


    // =========================================================
    // 문자열
    // =========================================================

    std::string toString() const override
    {
        std::ostringstream oss;

        oss << *this;

        return oss.str();
    }


    // =========================================================
    // 덧셈
    // =========================================================

    IrrationalNumber operator+(
        const IrrationalNumber& other
    ) const
    {
        IrrationalNumber result =
            *this;


        // 유리수 부분
        result.rationalPart +=
            other.rationalPart;


        // 근호 부분
        for (const auto& term :
             other.radicalTerms)
        {
            result.addTerm(
                term.coefficient,
                term.degree,
                term.radicand
            );
        }


        result.removeZeroTerms();

        return result;
    }


    // =========================================================
    // 뺄셈
    // =========================================================

    IrrationalNumber operator-(
        const IrrationalNumber& other
    ) const
    {
        IrrationalNumber result =
            *this;


        // 유리수 부분
        result.rationalPart -=
            other.rationalPart;


        // 근호 부분
        for (const auto& term :
             other.radicalTerms)
        {
            result.addTerm(
                Fraction<long long>(0)
                - term.coefficient,

                term.degree,
                term.radicand
            );
        }


        result.removeZeroTerms();

        return result;
    }


    // =========================================================
    // 곱셈
    //
    // root(a, x) * root(b, y)
    //
    // L = lcm(a, b)
    //
    // = root(
    //      L,
    //      x^(L/a) * y^(L/b)
    //   )
    // =========================================================

    IrrationalNumber operator*(
        const IrrationalNumber& other
    ) const
    {
        IrrationalNumber result;


        // -----------------------------------------------------
        // 유리수 * 유리수
        // -----------------------------------------------------

        result.rationalPart =
            rationalPart *
            other.rationalPart;


        // -----------------------------------------------------
        // 첫 번째 유리수 *
        // 두 번째 근호
        // -----------------------------------------------------

        for (const auto& term :
             other.radicalTerms)
        {
            result.addTerm(
                rationalPart *
                term.coefficient,

                term.degree,
                term.radicand
            );
        }


        // -----------------------------------------------------
        // 두 번째 유리수 *
        // 첫 번째 근호
        // -----------------------------------------------------

        for (const auto& term :
             radicalTerms)
        {
            result.addTerm(
                other.rationalPart *
                term.coefficient,

                term.degree,
                term.radicand
            );
        }


        // -----------------------------------------------------
        // 근호 * 근호
        // -----------------------------------------------------

        for (const auto& a :
             radicalTerms)
        {
            for (const auto& b :
                 other.radicalTerms)
            {
                long long commonDegree =
                    lcm(
                        a.degree,
                        b.degree
                    );


                long long leftPower =
                    commonDegree /
                    a.degree;

                long long rightPower =
                    commonDegree /
                    b.degree;


                // -------------------------------------------------
                // a.radicand ^ leftPower
                // -------------------------------------------------

                long long leftValue = 1;

                for (
                    long long i = 0;
                    i < leftPower;
                    ++i
                )
                {
                    if (
                        leftValue >
                        std::numeric_limits<
                            long long
                        >::max()
                        /
                        a.radicand
                    )
                    {
                        throw std::overflow_error(
                            "Radicand power overflow."
                        );
                    }

                    leftValue *=
                        a.radicand;
                }


                // -------------------------------------------------
                // b.radicand ^ rightPower
                // -------------------------------------------------

                long long rightValue = 1;

                for (
                    long long i = 0;
                    i < rightPower;
                    ++i
                )
                {
                    if (
                        rightValue >
                        std::numeric_limits<
                            long long
                        >::max()
                        /
                        b.radicand
                    )
                    {
                        throw std::overflow_error(
                            "Radicand power overflow."
                        );
                    }

                    rightValue *=
                        b.radicand;
                }


                // -------------------------------------------------
                // 두 radicand 곱
                // -------------------------------------------------

                if (
                    leftValue >
                    std::numeric_limits<
                        long long
                    >::max()
                    /
                    rightValue
                )
                {
                    throw std::overflow_error(
                        "Radicand multiplication overflow."
                    );
                }


                long long product =
                    leftValue *
                    rightValue;


                result.addTerm(
                    a.coefficient *
                    b.coefficient,

                    commonDegree,
                    product
                );
            }
        }


        result.removeZeroTerms();

        return result;
    }


    // =========================================================
    // 거듭제곱
    // =========================================================

    IrrationalNumber operator^(
        int exponent
    ) const
    {
        // -----------------------------------------------------
        // 0의 음수 지수
        // -----------------------------------------------------

        if (
            exponent < 0 &&
            rationalPart ==
                Fraction<long long>(0) &&
            radicalTerms.empty()
        )
        {
            throw std::invalid_argument(
                "Zero cannot have a negative exponent."
            );
        }


        // -----------------------------------------------------
        // 음수 지수
        // -----------------------------------------------------

        if (exponent < 0)
        {
            long long positiveExponent =
                -static_cast<long long>(
                    exponent
                );


            IrrationalNumber result =
                (*this) ^ static_cast<int>(
                    positiveExponent
                );


            return IrrationalNumber(1)
                / result;
        }


        // -----------------------------------------------------
        // 양수 지수
        // -----------------------------------------------------

        IrrationalNumber result(1);

        IrrationalNumber base =
            *this;

        int power = exponent;


        // -----------------------------------------------------
        // 분할 정복 거듭제곱
        // -----------------------------------------------------

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


    // =========================================================
    // 거듭제곱 복합 대입
    // =========================================================

    IrrationalNumber& operator^=(
        int exponent
    )
    {
        *this =
            *this ^ exponent;

        return *this;
    }


    // =========================================================
    // 유리수로 나누기
    // =========================================================

    IrrationalNumber operator/(
        const Fraction<long long>& divisor
    ) const
    {
        if (
            divisor ==
            Fraction<long long>(0)
        )
        {
            throw std::invalid_argument(
                "Division by zero."
            );
        }


        IrrationalNumber result;


        result.rationalPart =
            rationalPart /
            divisor;


        for (const auto& term :
             radicalTerms)
        {
            result.addTerm(
                term.coefficient /
                    divisor,

                term.degree,
                term.radicand
            );
        }


        result.removeZeroTerms();

        return result;
    }


    // =========================================================
    // 일반 정수로 나누기
    // =========================================================

    IrrationalNumber operator/(
        long long divisor
    ) const
    {
        return *this /
            Fraction<long long>(
                divisor
            );
    }


    // =========================================================
    // 무리수끼리 나누기
    //
    // 현재 단계에서는 기존의
    // "제곱근 기반 유리화"를 유지한다.
    //
    // 따라서 degree != 2인 근호가
    // 분모에 들어오면 아직 지원하지 않는다.
    // =========================================================

    IrrationalNumber operator/(
        const IrrationalNumber& divisor
    ) const
    {
        // -----------------------------------------------------
        // 0 확인
        // -----------------------------------------------------

        if (
            divisor.rationalPart ==
                Fraction<long long>(0) &&
            divisor.radicalTerms.empty()
        )
        {
            throw std::invalid_argument(
                "Division by zero."
            );
        }


        // -----------------------------------------------------
        // 분모가 유리수뿐이면
        // 기존 유리수 나눗셈
        // -----------------------------------------------------

        if (
            divisor.radicalTerms.empty()
        )
        {
            return *this /
                divisor.rationalPart;
        }


        // -----------------------------------------------------
        // 현재 유리화 알고리즘은
        // 제곱근에 대해서만 지원
        // -----------------------------------------------------

        for (
            const auto& term :
            divisor.radicalTerms
        )
        {
            if (term.degree != 2)
            {
                throw std::runtime_error(
                    "General radical rationalization "
                    "is not implemented yet."
                );
            }
        }


        /*
            기존 다중 켤레식 유리화

            1. 분모의 제곱근에서 소인수 p를 선택
            2. p를 포함하는 항의 부호를 반전
            3. 켤레식 생성
            4. 분모 * 켤레식
            5. 해당 radical 제거 확인
            6. 남은 분모를 재귀적으로 유리화
        */


        // -----------------------------------------------------
        // 첫 번째 근호에서 소인수 찾기
        // -----------------------------------------------------

        long long n =
            divisor.radicalTerms[0]
                .radicand;


        long long p = n;


        for (
            long long candidate = 2;
            candidate <= n / candidate;
            ++candidate
        )
        {
            if (n % candidate == 0)
            {
                p = candidate;
                break;
            }
        }


        // -----------------------------------------------------
        // 켤레식 생성
        // -----------------------------------------------------

        IrrationalNumber conjugate =
            divisor;


        for (
            auto& term :
            conjugate.radicalTerms
        )
        {
            if (
                term.radicand % p == 0
            )
            {
                term.coefficient =
                    Fraction<long long>(0)
                    -
                    term.coefficient;
            }
        }


        // -----------------------------------------------------
        // 분모 * 켤레식
        // -----------------------------------------------------

        IrrationalNumber norm =
            divisor *
            conjugate;


        // -----------------------------------------------------
        // 분모가 0인지 확인
        // -----------------------------------------------------

        if (
            norm.rationalPart ==
                Fraction<long long>(0) &&
            norm.radicalTerms.empty()
        )
        {
            throw std::invalid_argument(
                "Division by zero."
            );
        }


        // -----------------------------------------------------
        // 선택한 소인수가 들어간
        // 근호가 남아 있으면 실패
        // -----------------------------------------------------

        for (
            const auto& term :
            norm.radicalTerms
        )
        {
            if (
                term.radicand % p == 0
            )
            {
                throw std::runtime_error(
                    "Failed to rationalize "
                    "the denominator."
                );
            }
        }


        // -----------------------------------------------------
        // 분자 * 켤레식
        // -----------------------------------------------------

        IrrationalNumber numerator =
            *this *
            conjugate;


        // -----------------------------------------------------
        // 남은 분모 재귀 처리
        // -----------------------------------------------------

        return numerator /
            norm;
    }


    // =========================================================
    // 복합 대입
    // =========================================================

    IrrationalNumber& operator+=(
        const IrrationalNumber& other
    )
    {
        *this =
            *this + other;

        return *this;
    }


    IrrationalNumber& operator-=(
        const IrrationalNumber& other
    )
    {
        *this =
            *this - other;

        return *this;
    }


    IrrationalNumber& operator*=(
        const IrrationalNumber& other
    )
    {
        *this =
            *this * other;

        return *this;
    }


    IrrationalNumber& operator/=(
        const Fraction<long long>& divisor
    )
    {
        *this =
            *this / divisor;

        return *this;
    }


    IrrationalNumber& operator/=(
        const IrrationalNumber& divisor
    )
    {
        *this =
            *this / divisor;

        return *this;
    }


    IrrationalNumber& operator/=(
        long long divisor
    )
    {
        *this =
            *this / divisor;

        return *this;
    }


    // =========================================================
    // 비교
    // =========================================================

    bool operator==(
        const IrrationalNumber& other
    ) const
    {
        // -----------------------------------------------------
        // 유리수 부분
        // -----------------------------------------------------

        if (
            !(rationalPart ==
              other.rationalPart)
        )
        {
            return false;
        }


        // -----------------------------------------------------
        // 근호 항 개수
        // -----------------------------------------------------

        if (
            radicalTerms.size() !=
            other.radicalTerms.size()
        )
        {
            return false;
        }


        // -----------------------------------------------------
        // 각각의 항 비교
        // -----------------------------------------------------

        for (
            const auto& term :
            radicalTerms
        )
        {
            bool found = false;


            for (
                const auto& otherTerm :
                other.radicalTerms
            )
            {
                if (
                    term.degree ==
                        otherTerm.degree &&

                    term.radicand ==
                        otherTerm.radicand &&

                    term.coefficient ==
                        otherTerm.coefficient
                )
                {
                    found = true;
                    break;
                }
            }


            if (!found)
                return false;
        }


        return true;
    }


    bool operator!=(
        const IrrationalNumber& other
    ) const
    {
        return !(
            *this == other
        );
    }


    // =========================================================
    // 출력
    // =========================================================

    friend std::ostream& operator<<(
        std::ostream& os,
        const IrrationalNumber& number
    )
    {
        bool hasOutput = false;


        // -----------------------------------------------------
        // 유리수 부분
        // -----------------------------------------------------

        if (
            !(number.rationalPart ==
              Fraction<long long>(0))
            ||
            number.radicalTerms.empty()
        )
        {
            os <<
                number.rationalPart;

            hasOutput = true;
        }


        // -----------------------------------------------------
        // 근호 항
        // -----------------------------------------------------

        for (
            const auto& term :
            number.radicalTerms
        )
        {
            std::ostringstream
                coefficientStream;


            coefficientStream <<
                term.coefficient;


            std::string
                coefficientText =
                    coefficientStream.str();


            bool negative =
                !coefficientText.empty()
                &&
                coefficientText[0] == '-';


            // -------------------------------------------------
            // + / -
            // -------------------------------------------------

            if (hasOutput)
            {
                os <<
                    (
                        negative
                        ? " - "
                        : " + "
                    );
            }
            else if (negative)
            {
                os << "-";
            }


            // -------------------------------------------------
            // 계수 부호 제거
            // -------------------------------------------------

            if (negative)
            {
                coefficientText.erase(
                    0,
                    1
                );
            }


            // -------------------------------------------------
            // 계수 1이면 생략
            // -------------------------------------------------

            if (
                coefficientText != "1"
            )
            {
                os <<
                    coefficientText;
            }


            // -------------------------------------------------
            // 근호 출력
            // -------------------------------------------------

            if (term.degree == 2)
            {
                os <<
                    "sqrt("
                    << term.radicand
                    << ")";
            }
            else
            {
                os <<
                    "root("
                    << term.degree
                    << ","
                    << term.radicand
                    << ")";
            }


            hasOutput = true;
        }


        return os;
    }
};
