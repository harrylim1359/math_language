
#pragma once

#include "RationalNumber.h"
#include "IrrationalNumber.h"

#include <vector>
#include <utility>

// 유리수를 IrrationalNumber 표현으로 변환하는 내부 함수
namespace MathLangDetail
{
    inline IrrationalNumber toIrrational(
        const RationalNumber& number
    )
    {
        using FractionType = Fraction<long long>;
        using RadicalTermType =
            std::pair<FractionType, long long>;

        const std::vector<RadicalTermType> noTerms;

        return IrrationalNumber(
            number.getFraction(),
            noTerms
        );
    }
}

// ================================
// 덧셈
// ================================

inline IrrationalNumber operator+(
    const RationalNumber& lhs,
    const IrrationalNumber& rhs
)
{
    return MathLangDetail::toIrrational(lhs) + rhs;
}

inline IrrationalNumber operator+(
    const IrrationalNumber& lhs,
    const RationalNumber& rhs
)
{
    return lhs + MathLangDetail::toIrrational(rhs);
}

// ================================
// 뺄셈
// ================================

inline IrrationalNumber operator-(
    const RationalNumber& lhs,
    const IrrationalNumber& rhs
)
{
    return MathLangDetail::toIrrational(lhs) - rhs;
}

inline IrrationalNumber operator-(
    const IrrationalNumber& lhs,
    const RationalNumber& rhs
)
{
    return lhs - MathLangDetail::toIrrational(rhs);
}

// ================================
// 곱셈
// ================================

inline IrrationalNumber operator*(
    const RationalNumber& lhs,
    const IrrationalNumber& rhs
)
{
    return MathLangDetail::toIrrational(lhs) * rhs;
}

inline IrrationalNumber operator*(
    const IrrationalNumber& lhs,
    const RationalNumber& rhs
)
{
    return lhs * MathLangDetail::toIrrational(rhs);
}

// ================================
// 나눗셈
// ================================

inline IrrationalNumber operator/(
    const RationalNumber& lhs,
    const IrrationalNumber& rhs
)
{
    return MathLangDetail::toIrrational(lhs) / rhs;
}

inline IrrationalNumber operator/(
    const IrrationalNumber& lhs,
    const RationalNumber& rhs
)
{
    return lhs / rhs.getFraction();
}
