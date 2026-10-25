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
    // ��ȣ ��
    //
    // coefficient * root(degree, radicand)
    //
    // ��:
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
    // ������ �κ�
    // =========================================================

    Fraction<long long> rationalPart;


    // =========================================================
    // ��ȣ �� ���
    // =========================================================

    std::vector<RadicalTerm> radicalTerms;


    // =========================================================
    // �ִ�����
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
    // �ּҰ����
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
    // ��ȣ �ܼ�ȭ
    //
    // root(degree, n)
    //
    // ��
    //
    // coefficient * root(degree, radicand)
    //
    // ���·� ��ȯ
    //
    // ��:
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
        // ���μ�����
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
            // degree���� ���̸� ��ȣ ������ ����
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
            // �������� ��ȣ �ȿ� ����
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
        // ������ ���μ�
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
    // ��ȣ �� �߰�
    //
    // ������ �������� ����
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
        // ��ȣ �ܼ�ȭ
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
        // ���� ���� + ���� radicand�̸� ������
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
        // ���ο� �� �߰�
        // -----------------------------------------------------

        radicalTerms.emplace_back(
            newCoefficient,
            degree,
            newRadicand
        );
    }


    // =========================================================
    // ����� 0�� ��ȣ �� ����
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
    // �⺻ ������
    // =========================================================

    IrrationalNumber()
        : rationalPart(0)
    {
    }


    // =========================================================
    // ����
    // =========================================================

    IrrationalNumber(long long value)
        : rationalPart(value)
    {
    }


    // =========================================================
    // coefficient * root(degree, radicand)
    //
    // ��:
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
    // ��:
    //
    // IrrationalNumber(3, 2)
    //
    // -> cbrt(2)
    //
    // ����:
    // sqrt(2)�� ���� ��������
    // IrrationalNumber(2, true)�� ���
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
    // ���� ������ ������
    //
    // rational + coefficient * sqrt(radicand)
    //
    // ���� �׽�Ʈ���� ȣȯ���� ����
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
    // �ٻ�
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
    // ���ڿ�
    // =========================================================

    std::string toString() const override
    {
        std::ostringstream oss;

        oss << *this;

        return oss.str();
    }


    // =========================================================
    // ����
    // =========================================================

    IrrationalNumber operator+(
        const IrrationalNumber& other
    ) const
    {
        IrrationalNumber result =
            *this;


        // ������ �κ�
        result.rationalPart +=
            other.rationalPart;


        // ��ȣ �κ�
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
    // ����
    // =========================================================

    IrrationalNumber operator-(
        const IrrationalNumber& other
    ) const
    {
        IrrationalNumber result =
            *this;


        // ������ �κ�
        result.rationalPart -=
            other.rationalPart;


        // ��ȣ �κ�
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
    // ����
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
        // ������ * ������
        // -----------------------------------------------------

        result.rationalPart =
            rationalPart *
            other.rationalPart;


        // -----------------------------------------------------
        // ù ��° ������ *
        // �� ��° ��ȣ
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
        // �� ��° ������ *
        // ù ��° ��ȣ
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
        // ��ȣ * ��ȣ
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
                // �� radicand ��
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
    // �ŵ�����
    // =========================================================

    IrrationalNumber operator^(
        int exponent
    ) const
    {
        // -----------------------------------------------------
        // 0�� ���� ����
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
        // ���� ����
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
        // ��� ����
        // -----------------------------------------------------

        IrrationalNumber result(1);

        IrrationalNumber base =
            *this;

        int power = exponent;


        // -----------------------------------------------------
        // ���� ���� �ŵ�����
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
    // �ŵ����� ���� ����
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
    // �������� ������
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
    // �Ϲ� ������ ������
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
    // ���������� ������
    //
    // ���� �ܰ迡���� ������
    // "������ ��� ����ȭ"�� �����Ѵ�.
    //
    // ���� degree != 2�� ��ȣ��
    // �и� ������ ���� �������� �ʴ´�.
    // =========================================================

    IrrationalNumber operator/(
        const IrrationalNumber& divisor
    ) const
    {
        // -----------------------------------------------------
        // 0 Ȯ��
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
        // �и� ���������̸�
        // ���� ������ ������
        // -----------------------------------------------------

        if (
            divisor.radicalTerms.empty()
        )
        {
            return *this /
                divisor.rationalPart;
        }


        // -----------------------------------------------------
        // ���� ����ȭ �˰������
        // �����ٿ� ���ؼ��� ����
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
            ���� ���� �ӷ��� ����ȭ

            1. �и��� �����ٿ��� ���μ� p�� ����
            2. p�� �����ϴ� ���� ��ȣ�� ����
            3. �ӷ��� ����
            4. �и� * �ӷ���
            5. �ش� radical ���� Ȯ��
            6. ���� �и� ��������� ����ȭ
        */


        // -----------------------------------------------------
        // ù ��° ��ȣ���� ���μ� ã��
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
        // �ӷ��� ����
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
        // �и� * �ӷ���
        // -----------------------------------------------------

        IrrationalNumber norm =
            divisor *
            conjugate;


        // -----------------------------------------------------
        // �и� 0���� Ȯ��
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
        // ������ ���μ��� ��
        // ��ȣ�� ���� ������ ����
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
        // ���� * �ӷ���
        // -----------------------------------------------------

        IrrationalNumber numerator =
            *this *
            conjugate;


        // -----------------------------------------------------
        // ���� �и� ��� ó��
        // -----------------------------------------------------

        return numerator /
            norm;
    }


    // =========================================================
    // ���� ����
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
    // ��
    // =========================================================

    bool operator==(
        const IrrationalNumber& other
    ) const
    {
        // -----------------------------------------------------
        // ������ �κ�
        // -----------------------------------------------------

        if (
            !(rationalPart ==
              other.rationalPart)
        )
        {
            return false;
        }


        // -----------------------------------------------------
        // ��ȣ �� ����
        // -----------------------------------------------------

        if (
            radicalTerms.size() !=
            other.radicalTerms.size()
        )
        {
            return false;
        }


        // -----------------------------------------------------
        // ������ �� ��
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
    // ���
    // =========================================================

    friend std::ostream& operator<<(
        std::ostream& os,
        const IrrationalNumber& number
    )
    {
        bool hasOutput = false;


        // -----------------------------------------------------
        // ������ �κ�
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
        // ��ȣ ��
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
            // ��� ��ȣ ����
            // -------------------------------------------------

            if (negative)
            {
                coefficientText.erase(
                    0,
                    1
                );
            }


            // -------------------------------------------------
            // ��� 1�̸� ����
            // -------------------------------------------------

            if (
                coefficientText != "1"
            )
            {
                os <<
                    coefficientText;
            }


            // -------------------------------------------------
            // ��ȣ ���
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
