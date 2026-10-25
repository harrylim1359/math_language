
#pragma once

#include "RationalNumber.h"
#include "IrrationalNumber.h"

#include <vector>
#include <utility>

// �������� IrrationalNumber ǥ������ ��ȯ�ϴ� ���� �Լ�
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
// ����
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
// ����
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
// ����
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
// ������
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
// ȥ�� ���� ������
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

