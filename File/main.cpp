#include <cmath>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "NumberInclude.h"

namespace
{
int passed = 0;
int failed = 0;

template <typename T>
std::string stringify(const T& value)
{
    std::ostringstream out;
    out << value;
    return out.str();
}

template <typename T>
void checkString(const std::string& name, const T& value, const std::string& expected)
{
    const std::string actual = stringify(value);
    if (actual == expected)
    {
        std::cout << "[PASS] " << name << " -> " << actual << '\n';
        ++passed;
    }
    else
    {
        std::cout << "[FAIL] " << name << " -> expected: " << expected
                  << ", actual: " << actual << '\n';
        ++failed;
    }
}

void checkCondition(const std::string& name, bool condition)
{
    std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << '\n';
    condition ? ++passed : ++failed;
}

template <typename Function>
void checkException(const std::string& name, Function function)
{
    try
    {
        function();
        std::cout << "[FAIL] " << name << " -> exception was not thrown\n";
        ++failed;
    }
    catch (...)
    {
        std::cout << "[PASS] " << name << " -> exception thrown\n";
        ++passed;
    }
}
}

int main()
{
    using FractionValue = Fraction<long long>;

    checkString("fraction reduction", FractionValue(2, 4), "1/2");
    checkString("fraction addition", FractionValue(1, 2) + FractionValue(1, 3), "5/6");
    checkString("fraction multiplication", FractionValue(1, 2) * FractionValue(1, 3), "1/6");

    const FractionValue half(1, 2);
    checkString("e^(1/2) + e^(1/2)", E::value(half) + E::value(half), "2e^(1/2)");
    checkString("pi^(1/2) + pi^(1/2)", Pi::value(half) + Pi::value(half), "2pi^(1/2)");
    checkString("e + e", E::value() + E::value(), "2e");
    checkException("e + e^(1/2) is unsupported", [half]
    {
        (void)(E::value() + E::value(half));
    });
    checkCondition("e^(1/2) approximation",
                   std::abs(E::value(half).aprox() - std::sqrt(std::exp(1.0L))) < 1e-15L);
    checkCondition("pi^(1/2) approximation",
                   std::abs(Pi::value(half).aprox() - std::sqrt(std::acos(-1.0L))) < 1e-15L);

    checkString("sqrt(4)", SquareRoot::sqrt(4), "2");
    checkString("sqrt(8)", SquareRoot::sqrt(8), "2sqrt(2)");
    checkString("sqrt(12)", SquareRoot::sqrt(12), "2sqrt(3)");
    checkString("sqrt(18)", SquareRoot::sqrt(18), "3sqrt(2)");
    checkString("root(4,4) reduces to sqrt(2)", SquareRoot::sqrt(4, 4), "sqrt(2)");
    checkString("root(4,16) reduces to 2", SquareRoot::sqrt(4, 16), "2");
    checkString("root(4,64) reduces to 2sqrt(2)",
                SquareRoot::sqrt(4, 64), "2sqrt(2)");
    checkString("sqrt(2, sqrt(2, 2))",
                SquareRoot::sqrt(2, SquareRoot::sqrt(2, 2)), "root(4,2)");
    checkCondition("nested square root approximation",
                   std::abs(SquareRoot::sqrt(2, SquareRoot::sqrt(2, 2)).aprox()
                            - std::pow(2.0, 0.25)) < 1e-12);
    checkString("sqrt(1/2)", SquareRoot::sqrt(FractionValue(1, 2)), "1/2sqrt(2)");
    checkString("sqrt(0)", SquareRoot::sqrt(0), "0");
    checkCondition("sqrt(2) approximation",
                   std::abs(SquareRoot::sqrt(2).aprox() - std::sqrt(2.0)) < 1e-12);

    checkString("cbrt(8)", SquareRoot::cbrt(8), "2");
    checkString("cbrt(16)", SquareRoot::cbrt(16), "2root(3,2)");
    checkString("cbrt(54)", SquareRoot::cbrt(54), "3root(3,2)");
    checkString("cbrt(-8)", SquareRoot::cbrt(-8), "-2");
    checkString("cbrt(0)", SquareRoot::cbrt(0), "0");
    checkString("cbrt(1/8)", SquareRoot::cbrt(FractionValue(1, 8)), "1/2");
    checkString("cbrt(-1/8)", SquareRoot::cbrt(FractionValue(-1, 8)), "-1/2");
    checkString("cbrt(2/3)", SquareRoot::cbrt(FractionValue(2, 3)),
                "1/3root(3,18)");
    checkCondition("cbrt(2) approximation",
                   std::abs(SquareRoot::cbrt(2).aprox() - std::cbrt(2.0)) < 1e-12);
    checkCondition("cbrt(-2) approximation",
                   std::abs(SquareRoot::cbrt(-2).aprox() - std::cbrt(-2.0)) < 1e-12);

    checkException("negative square root", [] { (void)SquareRoot::sqrt(-1); });
    checkException("zero fraction denominator", [] { (void)FractionValue(1, 0); });

    std::cout << "\nPassed: " << passed << ", failed: " << failed
              << ", total: " << passed + failed << '\n';
    return failed == 0 ? 0 : 1;
}
