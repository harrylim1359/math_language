#pragma once

#include "ExactReal.h"
#include "Or.h"

#include <cmath>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

class ComplexNumber : public Number
{
private:
    ExactReal realPart;
    ExactReal imaginaryPart;

    static ExactReal convert(const RealNumber& value)
    {
        if (const auto* exact = dynamic_cast<const ExactReal*>(&value)) return *exact;
        if (const auto* rational = dynamic_cast<const RationalNumber*>(&value)) return ExactReal(*rational);
        if (const auto* irrational = dynamic_cast<const IrrationalNumber*>(&value)) return ExactReal(*irrational);
        throw std::invalid_argument("Unsupported real-number implementation.");
    }

    static ExactReal convert(const std::shared_ptr<RealNumber>& value)
    {
        if (!value) throw std::invalid_argument("ComplexNumber parts cannot be null.");
        return convert(*value);
    }

public:
    ComplexNumber() = default;
    ComplexNumber(long long real) : realPart(real) {}
    ComplexNumber(const BigInteger& real) : realPart(real) {}
    ComplexNumber(const RationalNumber& real) : realPart(real) {}
    ComplexNumber(const IrrationalNumber& real) : realPart(real) {}
    ComplexNumber(ExactReal real, ExactReal imaginary = ExactReal(0))
        : realPart(std::move(real)), imaginaryPart(std::move(imaginary)) {}
    ComplexNumber(const RealNumber& real) : realPart(convert(real)) {}

    // Compatibility with the former FakeNumber constructor.
    ComplexNumber(std::shared_ptr<RealNumber> real,
                  std::shared_ptr<RealNumber> imaginary)
        : realPart(convert(real)), imaginaryPart(convert(imaginary)) {}

    const RealNumber& getRealPart() const { return realPart; }
    const RealNumber& getImaginaryPart() const { return imaginaryPart; }
    const ExactReal& real() const { return realPart; }
    const ExactReal& imaginary() const { return imaginaryPart; }

    ComplexNumber conjugate() const
    { return ComplexNumber(realPart, ExactReal(0) - imaginaryPart); }

    ExactReal magnitude() const
    { return ExactReal::root(2, realPart * realPart + imaginaryPart * imaginaryPart); }

    static ComplexNumber exp(const ComplexNumber& z)
    {
        const ExactReal scale=ExactReal::exp(z.realPart);
        return ComplexNumber(scale*ExactReal::cos(z.imaginaryPart),
                             scale*ExactReal::sin(z.imaginaryPart));
    }

    static ComplexNumber log(const ComplexNumber& z, const BigInteger& branch=0)
    {
        if(z.realPart.isZero() && z.imaginaryPart.isZero())
            throw std::invalid_argument("Complex logarithm is undefined at zero.");
        const ExactReal angle=ExactReal::atan2(z.imaginaryPart,z.realPart)
                            + ExactReal(2)*ExactReal::pi()*ExactReal(branch);
        return ComplexNumber(ExactReal::log(z.magnitude()),angle);
    }

    static Or<ComplexNumber> logarithms(const ComplexNumber& z)
    {
        if(z.realPart.isZero() && z.imaginaryPart.isZero())
            throw std::invalid_argument("Complex logarithm is undefined at zero.");
        return Or<ComplexNumber>::lazy([z](const BigInteger& k){ return log(z,k); });
    }

    static Or<ComplexNumber> roots(const ComplexNumber& z,const BigInteger& degree)
    {
        if(degree<=0) throw std::invalid_argument("Root degree must be positive.");
        if(degree==1) return Or<ComplexNumber>(std::vector<ComplexNumber>{z});
        if(z.realPart.isZero() && z.imaginaryPart.isZero())
            return Or<ComplexNumber>(std::vector<ComplexNumber>{ComplexNumber(0)});
        std::vector<ComplexNumber> values;
        const ExactReal rho=ExactReal::root(degree,z.magnitude());
        const ExactReal theta=ExactReal::atan2(z.imaginaryPart,z.realPart);
        for(BigInteger k=0;k<degree;++k)
        {
            const ExactReal angle=(theta+ExactReal(2)*ExactReal::pi()*ExactReal(k))/ExactReal(degree);
            values.emplace_back(rho*ExactReal::cos(angle),rho*ExactReal::sin(angle));
        }
        return Or<ComplexNumber>(std::move(values));
    }

    Or<ComplexNumber> power(const ComplexNumber& exponent) const
    {
        // z^w = exp(w Log(z)); Log supplies every branch lazily.
        const ComplexNumber z(*this);
        const ComplexNumber w(exponent);
        if(w.imaginaryPart.isZero())
            if(auto integer=w.realPart.integerValue())
                return Or<ComplexNumber>(std::vector<ComplexNumber>{*this ^ *integer});
        if(z.realPart.isZero() && z.imaginaryPart.isZero())
            throw std::invalid_argument("Zero cannot be raised to a non-integer complex power.");
        return Or<ComplexNumber>::lazy([z,w](const BigInteger& k){ return exp(w*log(z,k)); });
    }

    double aprox() const override
    { return std::hypot(realPart.aprox(), imaginaryPart.aprox()); }
    Decimal aprox100() const override
    { const Decimal r=realPart.aprox100(), i=imaginaryPart.aprox100(); return sqrt(r*r+i*i); }

    std::string toString() const override
    {
        if (imaginaryPart.isZero()) return realPart.toString();
        if (realPart.isZero()) return "(" + imaginaryPart.toString() + ")i";
        return "(" + realPart.toString() + " + (" + imaginaryPart.toString() + ")i)";
    }

    ComplexNumber operator+(const ComplexNumber& other) const
    { return ComplexNumber(realPart + other.realPart, imaginaryPart + other.imaginaryPart); }
    ComplexNumber operator-(const ComplexNumber& other) const
    { return ComplexNumber(realPart - other.realPart, imaginaryPart - other.imaginaryPart); }
    ComplexNumber operator*(const ComplexNumber& other) const
    {
        return ComplexNumber(realPart * other.realPart - imaginaryPart * other.imaginaryPart,
                             realPart * other.imaginaryPart + imaginaryPart * other.realPart);
    }
    ComplexNumber operator/(const ComplexNumber& other) const
    {
        const ExactReal denominator = other.realPart * other.realPart
                                    + other.imaginaryPart * other.imaginaryPart;
        if (denominator.isZero()) throw std::invalid_argument("Division by zero.");
        return ComplexNumber((realPart * other.realPart + imaginaryPart * other.imaginaryPart) / denominator,
                             (imaginaryPart * other.realPart - realPart * other.imaginaryPart) / denominator);
    }

    ComplexNumber operator^(const BigInteger& exponent) const
    {
        if (exponent == 0) return ComplexNumber(1);
        const bool negative = exponent < 0;
        BigInteger power = negative ? -exponent : exponent;
        ComplexNumber result(1), base = *this;
        while (power)
        {
            if ((power & 1) != 0) result = result * base;
            power >>= 1;
            if (power) base = base * base;
        }
        return negative ? ComplexNumber(1) / result : result;
    }
    ComplexNumber operator^(int exponent) const { return *this ^ BigInteger(exponent); }

    bool operator==(const ComplexNumber& other) const
    { return realPart == other.realPart && imaginaryPart == other.imaginaryPart; }
    bool operator!=(const ComplexNumber& other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream& out, const ComplexNumber& value)
    { return out << value.toString(); }
};

inline ComplexNumber operator+(const RealNumber& lhs, const ComplexNumber& rhs)
{ return ComplexNumber(ExactReal(lhs)) + rhs; }
inline ComplexNumber operator-(const RealNumber& lhs, const ComplexNumber& rhs)
{ return ComplexNumber(ExactReal(lhs)) - rhs; }
inline ComplexNumber operator*(const RealNumber& lhs, const ComplexNumber& rhs)
{ return ComplexNumber(ExactReal(lhs)) * rhs; }
inline ComplexNumber operator/(const RealNumber& lhs, const ComplexNumber& rhs)
{ return ComplexNumber(ExactReal(lhs)) / rhs; }

inline bool approxEqual(const ExactReal& a,const ExactReal& b,const ExactReal::Decimal& tolerance)
{
    if(tolerance<0) throw std::invalid_argument("Tolerance must be nonnegative.");
    using boost::multiprecision::abs;
    return abs(a.aprox100()-b.aprox100())<=tolerance;
}
inline bool approxEqual(const ComplexNumber& a,const ComplexNumber& b,const ComplexNumber::Decimal& tolerance)
{
    if(tolerance<0) throw std::invalid_argument("Tolerance must be nonnegative.");
    using boost::multiprecision::sqrt;
    const auto dr=a.real().aprox100()-b.real().aprox100();
    const auto di=a.imaginary().aprox100()-b.imaginary().aprox100();
    return sqrt(dr*dr+di*di)<=tolerance;
}

inline Or<ComplexNumber> operator+(const Or<ComplexNumber>& a,const Or<ComplexNumber>& b)
{ return orCombine(a,b,[](const auto& x,const auto& y){return x+y;}); }
inline Or<ComplexNumber> operator-(const Or<ComplexNumber>& a,const Or<ComplexNumber>& b)
{ return orCombine(a,b,[](const auto& x,const auto& y){return x-y;}); }
inline Or<ComplexNumber> operator*(const Or<ComplexNumber>& a,const Or<ComplexNumber>& b)
{ return orCombine(a,b,[](const auto& x,const auto& y){return x*y;}); }
inline Or<ComplexNumber> operator/(const Or<ComplexNumber>& a,const Or<ComplexNumber>& b)
{ return orCombine(a,b,[](const auto& x,const auto& y){return x/y;}); }
