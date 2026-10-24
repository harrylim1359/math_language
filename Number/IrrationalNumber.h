#pragma once

#include "RealNumber.h"
#include "Fraction.h"

#include <vector>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>

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

    Fraction<long long> rationalPart;
    std::vector<RadicalTerm> radicalTerms;


    // √n을
    //
    // a√b
    //
    // 형태로 변환
    static void simplifyRadical(
        long long radicand,
        long long& coefficient,
        long long& simplifiedRadicand
    )
    {
        if (radicand <= 0)
        {
            throw std::invalid_argument(
                "Radicand must be positive."
            );
        }

        coefficient = 1;
        simplifiedRadicand = radicand;

        for (long long i = 2; i * i <= simplifiedRadicand; ++i)
        {
            long long square = i * i;

            while (simplifiedRadicand % square == 0)
            {
                coefficient *= i;
                simplifiedRadicand /= square;
            }
        }
    }


    // 같은 √n 항을 합침
    void combineLikeTerms()
    {
        for (size_t i = 0; i < radicalTerms.size(); ++i)
        {
            for (size_t j = i + 1; j < radicalTerms.size();)
            {
                if (radicalTerms[i].radicand
                    == radicalTerms[j].radicand)
                {
                    radicalTerms[i].coefficient
                        += radicalTerms[j].coefficient;

                    radicalTerms.erase(
                        radicalTerms.begin() + j
                    );
                }
                else
                {
                    ++j;
                }
            }
        }

        // 계수가 0인 항 제거
        for (size_t i = 0; i < radicalTerms.size();)
        {
            if (radicalTerms[i].coefficient
                == Fraction<long long>(0))
            {
                radicalTerms.erase(
                    radicalTerms.begin() + i
                );
            }
            else
            {
                ++i;
            }
        }
    }


public:

    IrrationalNumber()
        : rationalPart(0)
    {
    }


    // √radicand
    explicit IrrationalNumber(long long radicand)
        : rationalPart(0)
    {
        long long coefficient;
        long long simplifiedRadicand;

        simplifyRadical(
            radicand,
            coefficient,
            simplifiedRadicand
        );

        radicalTerms.emplace_back(
            Fraction<long long>(coefficient),
            simplifiedRadicand
        );
    }


    // 계수 × √radicand
    IrrationalNumber(
        const Fraction<long long>& coefficient,
        long long radicand
    )
        : rationalPart(0)
    {
        long long radicalCoefficient;
        long long simplifiedRadicand;

        simplifyRadical(
            radicand,
            radicalCoefficient,
            simplifiedRadicand
        );

        Fraction<long long> finalCoefficient =
            coefficient
            * Fraction<long long>(radicalCoefficient);

        radicalTerms.emplace_back(
            finalCoefficient,
            simplifiedRadicand
        );

        combineLikeTerms();
    }


    // 근삿값
    double aprox() const override
    {
        double result = rationalPart.aprox();

        for (const auto& term : radicalTerms)
        {
            result +=
                term.coefficient.aprox()
                * std::sqrt(
                    static_cast<double>(term.radicand)
                );
        }

        return result;
    }


    // 항 개수
    size_t size() const
    {
        return radicalTerms.size();
    }


    // 출력
    friend std::ostream& operator<<(
        std::ostream& os,
        const IrrationalNumber& number
    )
    {
        bool first = true;

        // 유리수 부분
        if (number.rationalPart
            != Fraction<long long>(0))
        {
            os << number.rationalPart;
            first = false;
        }

        // 무리수 항
        for (const auto& term : number.radicalTerms)
        {
            Fraction<long long> coefficient =
                term.coefficient;

            if (!first)
            {
                if (coefficient.aprox() >= 0)
                    os << " + ";
                else
                    os << " - ";
            }
            else if (coefficient.aprox() < 0)
            {
                os << "-";
            }

            // 부호 제거 후 출력
            if (coefficient.aprox() < 0)
            {
                coefficient =
                    Fraction<long long>(
                        -coefficient.getNumerator(),
                        coefficient.getDenominator()
                    );
            }

            if (coefficient != Fraction<long long>(1))
            {
                os << coefficient;
            }

            os << "sqrt("
               << term.radicand
               << ")";

            first = false;
        }

        if (first)
            os << "0";

        return os;
    }

    std::string toString() const override
    {
        std::ostringstream oss;
        oss << *this;
        return oss.str();
    }
};
