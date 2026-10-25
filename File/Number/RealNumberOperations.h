
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
        return IrrationalNumber(number.getFraction());
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


 // ================================
// 혼합 대입 연산자
// ================================

// IrrationalNumber += RationalNumber
inline IrrationalNumber& operator+=(
    IrrationalNumber& lhs,
    const RationalNumber& rhs
)
{
    lhs = lhs + MathLangDetail::toIrrational(rhs);
    return lhs;
}

// IrrationalNumber -= RationalNumber
inline IrrationalNumber& operator-=(
    IrrationalNumber& lhs,
    const RationalNumber& rhs
)
{
    lhs = lhs - MathLangDetail::toIrrational(rhs);
    return lhs;
}

// IrrationalNumber *= RationalNumber
inline IrrationalNumber& operator*=(
    IrrationalNumber& lhs,
    const RationalNumber& rhs
)
{
    lhs = lhs * MathLangDetail::toIrrational(rhs);
    return lhs;
}

// IrrationalNumber /= RationalNumber
inline IrrationalNumber& operator/=(
    IrrationalNumber& lhs,
    const RationalNumber& rhs
)
{
    lhs = lhs / rhs.getFraction();
    return lhs;
}

