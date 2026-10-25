#include <iostream>
#include <string>
#include <stdexcept>

#include "NumberInclude.h"

using namespace std;

int passCount = 0;
int failCount = 0;


// =========================================================
// 테스트 함수
// =========================================================

void test(
    const string& name,
    const IrrationalNumber& actual,
    const string& expected
)
{
    string actualString = actual.toString();

    if (actualString == expected)
    {
        cout << "[PASS] "
             << name
             << " -> "
             << actualString
             << endl;

        passCount++;
    }
    else
    {
        cout << "[FAIL] "
             << name
             << " -> 예상: "
             << expected
             << ", 실제: "
             << actualString
             << endl;

        failCount++;
    }
}

template <typename T>
void testValue(const string& name, const T& actual, const string& expected)
{
    const string actualString = actual.toString();
    if (actualString == expected)
    {
        cout << "[PASS] " << name << " -> " << actualString << endl;
        passCount++;
    }
    else
    {
        cout << "[FAIL] " << name << " -> expected: " << expected
             << ", actual: " << actualString << endl;
        failCount++;
    }
}

void testCondition(const string& name, bool passed)
{
    cout << (passed ? "[PASS] " : "[FAIL] ") << name << endl;
    passed ? passCount++ : failCount++;
}


template <typename Function>
void testException(
    const string& name,
    Function function
)
{
    try
    {
        function();

        cout << "[FAIL] "
             << name
             << " -> 예외가 발생하지 않음"
             << endl;

        failCount++;
    }
    catch (...)
    {
        cout << "[PASS] "
             << name
             << " -> 예외 발생"
             << endl;

        passCount++;
    }
}


// =========================================================
// 테스트 시작
// =========================================================

int main()
{
    cout << "===== 일반 n차 근호 자동 테스트 ====="
         << endl
         << endl;


    // =====================================================
    // 1. 기본 n차 근호 생성
    // =====================================================

    test(
        "정수 5",
        IrrationalNumber(5),
        "5"
    );

    test(
        "sqrt(2)",
        IrrationalNumber(2, 2),
        "sqrt(2)"
    );

    test(
        "sqrt(3)",
        IrrationalNumber(2, 3),
        "sqrt(3)"
    );

    test(
        "cbrt(2)",
        IrrationalNumber(3, 2),
        "root(3,2)"
    );

    test(
        "root(5,7)",
        IrrationalNumber(5, 7),
        "root(5,7)"
    );


    // =====================================================
    // 2. 제곱근 단순화
    // =====================================================

    test(
        "sqrt(4) = 2",
        IrrationalNumber(2, 4),
        "2"
    );

    test(
        "sqrt(8) = 2sqrt(2)",
        IrrationalNumber(2, 8),
        "2sqrt(2)"
    );

    test(
        "sqrt(12) = 2sqrt(3)",
        IrrationalNumber(2, 12),
        "2sqrt(3)"
    );

    test(
        "sqrt(18) = 3sqrt(2)",
        IrrationalNumber(2, 18),
        "3sqrt(2)"
    );


    // =====================================================
    // 3. 세제곱근 단순화
    // =====================================================

    test(
        "cbrt(8) = 2",
        IrrationalNumber(3, 8),
        "2"
    );

    test(
        "cbrt(16) = 2root(3,2)",
        IrrationalNumber(3, 16),
        "2root(3,2)"
    );

    test(
        "cbrt(54) = 3root(3,2)",
        IrrationalNumber(3, 54),
        "3root(3,2)"
    );


    // =====================================================
    // 4. 5차근 단순화
    // =====================================================

    test(
        "root(5,32) = 2",
        IrrationalNumber(5, 32),
        "2"
    );

    test(
        "root(5,64) = 2root(5,2)",
        IrrationalNumber(5, 64),
        "2root(5,2)"
    );


    // =====================================================
    // 5. 같은 근호의 덧셈
    // =====================================================

    IrrationalNumber cbrt2(3, 2);

    test(
        "cbrt(2) + cbrt(2)",
        cbrt2 + cbrt2,
        "2root(3,2)"
    );

    test(
        "sqrt(2) + sqrt(2)",
        IrrationalNumber(2, 2)
        + IrrationalNumber(2, 2),
        "2sqrt(2)"
    );


    // =====================================================
    // 6. 같은 근호의 뺄셈
    // =====================================================

    test(
        "cbrt(2) - cbrt(2)",
        cbrt2 - cbrt2,
        "0"
    );

    test(
        "sqrt(2) - sqrt(2)",
        IrrationalNumber(2, 2)
        - IrrationalNumber(2, 2),
        "0"
    );


    // =====================================================
    // 7. 일반 근호 곱셈
    // =====================================================

    test(
        "cbrt(2) * cbrt(2)",
        IrrationalNumber(3, 2)
        * IrrationalNumber(3, 2),
        "root(3,4)"
    );

    test(
        "cbrt(2) * cbrt(4)",
        IrrationalNumber(3, 2)
        * IrrationalNumber(3, 4),
        "2"
    );

    test(
        "sqrt(2) * sqrt(3)",
        IrrationalNumber(2, 2)
        * IrrationalNumber(2, 3),
        "sqrt(6)"
    );

    test(
        "sqrt(2) * sqrt(8)",
        IrrationalNumber(2, 2)
        * IrrationalNumber(2, 8),
        "4"
    );


    // =====================================================
    // 8. 서로 다른 차수의 근호 곱셈
    // =====================================================

    test(
        "sqrt(2) * cbrt(2)",
        IrrationalNumber(2, 2)
        * IrrationalNumber(3, 2),
        "root(6,32)"
    );


    // =====================================================
    // 9. 계수가 있는 일반 근호
    // =====================================================

    test(
        "2cbrt(2)",
        IrrationalNumber(
            Fraction<long long>(2),
            3,
            2
        ),
        "2root(3,2)"
    );

    test(
        "1/2 sqrt(2)",
        IrrationalNumber(
            Fraction<long long>(1, 2),
            2,
            2
        ),
        "1/2sqrt(2)"
    );


    // =====================================================
    // 10. 거듭제곱
    // =====================================================

    test(
        "sqrt(2)^2",
        IrrationalNumber(2, 2) ^ 2,
        "2"
    );

    test(
        "sqrt(2)^3",
        IrrationalNumber(2, 2) ^ 3,
        "2sqrt(2)"
    );

    test(
        "cbrt(2)^2",
        IrrationalNumber(3, 2) ^ 2,
        "root(3,4)"
    );

    test(
        "cbrt(2)^3",
        IrrationalNumber(3, 2) ^ 3,
        "2"
    );

    test(
        "root(5,2)^5",
        IrrationalNumber(5, 2) ^ 5,
        "2"
    );


    // =====================================================
    // 11. 음의 거듭제곱
    // =====================================================

    test(
        "sqrt(2)^-2",
        IrrationalNumber(2, 2) ^ -2,
        "1/2"
    );


    // =====================================================
    // 12. 유리수와 일반 근호 혼합
    // =====================================================

    test(
        "2 + cbrt(2)",
        IrrationalNumber(2)
        + IrrationalNumber(3, 2),
        "2 + root(3,2)"
    );

    test(
        "3 - cbrt(2)",
        IrrationalNumber(3)
        - IrrationalNumber(3, 2),
        "3 - root(3,2)"
    );

    test(
        "2 * cbrt(2)",
        IrrationalNumber(2)
        * IrrationalNumber(3, 2),
        "2root(3,2)"
    );


    // =====================================================
    // 13. 잘못된 근호
    // =====================================================

    testException(
        "root(1,2)",
        []()
        {
            IrrationalNumber invalid(1, 2);
        }
    );

    testException(
        "root(0,2)",
        []()
        {
            IrrationalNumber invalid(0, 2);
        }
    );

    testException(
        "root(3,0)",
        []()
        {
            IrrationalNumber invalid(3, 0);
        }
    );

    // Exact division by arbitrary-degree radicals.
    testValue("cbrt(2) / cbrt(3)",
        IrrationalNumber(3, 2) / IrrationalNumber(3, 3),
        "1/3root(3,18)");
    testValue("root(5,2) / root(5,3)",
        IrrationalNumber(5, 2) / IrrationalNumber(5, 3),
        "1/3root(5,162)");
    testValue("1 / (1 + cbrt(2))",
        IrrationalNumber(1) / (IrrationalNumber(1) + IrrationalNumber(3, 2)),
        "1/3 - 1/3root(3,2) + 1/3root(3,4)");
    testValue("1 / (sqrt(2) + sqrt(3))",
        IrrationalNumber(1) /
            (IrrationalNumber(2, 2) + IrrationalNumber(2, 3)),
        "-sqrt(2) + sqrt(3)");
    testValue("1 / (cbrt(2) + cbrt(3))",
        IrrationalNumber(1) /
            (IrrationalNumber(3, 2) + IrrationalNumber(3, 3)),
        "1/5root(3,4) - 1/5root(3,6) + 1/5root(3,9)");
    const ExactReal threeRootDenominator(
        IrrationalNumber(3, 2) + IrrationalNumber(3, 3) + IrrationalNumber(3, 5));
    const ExactReal preservedQuotient = ExactReal(1) / threeRootDenominator;
    testCondition("unsupported multi-root denominator stays exact",
        preservedQuotient.toString().find(" / ") != string::npos
        && std::abs(preservedQuotient.aprox()
            - 1.0 / threeRootDenominator.aprox()) < 1e-12);

    testException("radical division by zero", []()
    {
        (void)(IrrationalNumber(2, 2) / IrrationalNumber(0));
    });

    const BigInteger huge("123456789012345678901234567890");
    const RationalNumber hugeFraction(huge, BigInteger(10));
    testValue("arbitrary precision fraction", hugeFraction,
        "12345678901234567890123456789");
    testValue("arbitrary precision radical simplification",
        IrrationalNumber(BigInteger(2), BigInteger(1) << 100),
        "1125899906842624");

    const ExactReal pi = ExactReal::pi();
    const ExactReal e = ExactReal::e();
    testValue("exact pi symbol", pi, "pi");
    testValue("exact e symbol", e, "e");
    testValue("symbolic pi + e", pi + e, "(pi + e)");
    testValue("exact rational cube root",
        ExactReal::root(3, ExactReal(Fraction<BigInteger>(1, 2))),
        "1/2root(3,4)");
    testCondition("pi approximation", std::abs(pi.aprox() - 3.141592653589793) < 1e-15);
    testCondition("e approximation", std::abs(e.aprox() - 2.718281828459045) < 1e-15);

    const ComplexNumber z(ExactReal(1), ExactReal(2));
    testValue("complex addition", z + ComplexNumber(3, 4), "(4 + (6)i)");
    testValue("complex multiplication", z * ComplexNumber(3, -4), "(11 + (2)i)");
    testValue("complex division", z / ComplexNumber(3, 4), "(11/25 + (2/25)i)");
    testValue("complex conjugate", z.conjugate(), "(1 + (-2)i)");
    testValue("complex integer power", z ^ 2, "(-3 + (4)i)");
    testValue("complex magnitude", z.magnitude(), "sqrt(5)");
    testCondition("complex magnitude approximation", std::abs(z.magnitude().aprox() - std::sqrt(5.0)) < 1e-12);
    testValue("complex negative integer power", z ^ -1, "(1/5 + (-2/5)i)");
    testCondition("big integer complex power remains single valued",
        (ComplexNumber(1).power(ComplexNumber(BigInteger("100000000000000000000000000000000000000000000000001"))).size()==1));
    const ComplexNumber symbolicComplex(pi, e);
    testCondition("pi/e complex approximation",
        std::abs(symbolicComplex.aprox() - std::hypot(pi.aprox(), e.aprox())) < 1e-12);
    testValue("rational mixed with exact real",
        RationalNumber(2) + pi, "(2 + pi)");
    testValue("real mixed with complex",
        RationalNumber(2) + ComplexNumber(1, 3), "(3 + (3)i)");
    testCondition("FakeNumber compatibility alias",
        FakeNumber(1, 2).toString() == ComplexNumber(1, 2).toString());
    testCondition("FakeNumber shared-pointer compatibility",
        FakeNumber(std::make_shared<RationalNumber>(1),
                   std::make_shared<RationalNumber>(2)).toString()
            == "(1 + (2)i)");
    testException("complex division by zero", []()
    {
        (void)(ComplexNumber(1, 2) / ComplexNumber(0));
    });

    testCondition("canonical commutative pi/e equality", (pi+e)==(e+pi));
    testCondition("symbolic rational coefficients combine", (ExactReal(2)*pi+ExactReal(3)*pi)==ExactReal(5)*pi);
    testCondition("unknown symbolic identity is not claimed", !(ExactReal::exp(ExactReal(1))==e));
    const ExactReal::Decimal sqrtTwo100("1.4142135623730950488016887242096980785696718753769480731766797379907324784621070388503875343276415727");
    testCondition("100 digit sqrt approximation", boost::multiprecision::abs(ExactReal::root(2,ExactReal(2)).aprox100()-sqrtTwo100)<ExactReal::Decimal("1e-99"));
    testCondition("absolute approximate equality", approxEqual(ExactReal(1),ExactReal(1)+ExactReal(Fraction<BigInteger>(1,BigInteger(1000000))),ExactReal::Decimal("0.000001")));
    testCondition("complex absolute approximate equality", approxEqual(ComplexNumber(1,1),ComplexNumber(ExactReal(1)+ExactReal(Fraction<BigInteger>(1,BigInteger(10000000))),1),ComplexNumber::Decimal("0.0000001")));
    testCondition("negative tolerance rejected", [&](){ try { (void)approxEqual(ExactReal(1),ExactReal(1),ExactReal::Decimal(-1)); return false; } catch(...) { return true; } }());
    testCondition("negative odd real root", std::abs(ExactReal::root(3,ExactReal(-8)).aprox()+2.0)<1e-12);
    testValue("negative perfect odd root simplifies", ExactReal::root(3,ExactReal(-8)), "-2");
    const auto zeroRoots=ComplexNumber::roots(ComplexNumber(0),5);
    testCondition("zero root has one distinct branch", !zeroRoots.isInfinite() && zeroRoots.size()==1 && zeroRoots.at(0)==ComplexNumber(0));
    const auto squareRoots=ComplexNumber::roots(ComplexNumber(-1),2);
    testCondition("complex square root has both branches", !squareRoots.isInfinite() && squareRoots.size()==2);
    testCondition("complex square root branch values", approxEqual(squareRoots.at(0),ComplexNumber(ExactReal(0),ExactReal(1)),ComplexNumber::Decimal("1e-90")) && approxEqual(squareRoots.at(1),ComplexNumber(ExactReal(0),ExactReal(-1)),ComplexNumber::Decimal("1e-90")));
    const auto negativeCubeRoots=ComplexNumber::roots(ComplexNumber(-8),3);
    testCondition("negative real cube root included among complex branches", approxEqual(negativeCubeRoots.at(1),ComplexNumber(-2),ComplexNumber::Decimal("1e-90")));
    const auto rootSums=squareRoots+squareRoots;
    testCondition("Or arithmetic distributes over choices", !rootSums.isInfinite() && rootSums.size()==4);
    const auto logBranches=ComplexNumber::logarithms(ComplexNumber(1));
    testCondition("complex log branches are lazy", logBranches.isInfinite());
    testCondition("complex log supports negative branch indices",
        approxEqual(logBranches.branch(-1),ComplexNumber(ExactReal(0),ExactReal(-2)*pi),ComplexNumber::Decimal("1e-90")));
    const auto logSums=logBranches+logBranches;
    testCondition("infinite Or arithmetic reaches negative branches",
        approxEqual(logSums.branch(-1),logBranches.branch(-1),ComplexNumber::Decimal("1e-90")));
    testCondition("complex exp high precision", approxEqual(ComplexNumber::exp(ComplexNumber(1)),ComplexNumber(ExactReal::e()),ComplexNumber::Decimal("1e-90")));
    const auto eulerExpression=ComplexNumber::exp(ComplexNumber(ExactReal(0),pi))+ComplexNumber(1);
    testCondition("Euler identity remains unproven exactly", !(eulerExpression==ComplexNumber(0)));
    testCondition("Euler identity is close numerically", approxEqual(eulerExpression,ComplexNumber(0),ComplexNumber::Decimal("1e-90")));
    testException("complex log zero", [](){ (void)ComplexNumber::logarithms(ComplexNumber(0)); });


    // =====================================================
    // 테스트 결과
    // =====================================================

    cout << endl;
    cout << "===== 테스트 결과 ====="
         << endl;

    cout << "통과: "
         << passCount
         << endl;

    cout << "실패: "
         << failCount
         << endl;

    cout << "전체: "
         << passCount + failCount
         << endl;

    cout << endl;

    if (failCount == 0)
    {
        cout << "모든 테스트 통과!"
             << endl;

        return 0;
    }

    cout << "일부 테스트 실패"
         << endl;

    return 1;
}
