#pragma once

#include "IrrationalNumber.h"
#include "RationalNumber.h"

#include <cmath>
#include <memory>
#include <numbers>
#include <sstream>
#include <stdexcept>
#include <string>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <algorithm>
#include <vector>
#include <optional>
#include <boost/math/constants/constants.hpp>

class ExactReal : public RealNumber
{
public:
    using Decimal = boost::multiprecision::cpp_dec_float_100;
private:
    enum class Kind { Rational, Pi, Euler, Add, Subtract, Multiply, Divide, Power, Root, Exp, Log, Sin, Cos, Atan2 };
    struct Node
    {
        Kind kind;
        Fraction<BigInteger> rational{0};
        std::shared_ptr<const Node> left;
        std::shared_ptr<const Node> right;
        int exponent = 0;
        BigInteger degree = 0;

        explicit Node(const Fraction<BigInteger>& value) : kind(Kind::Rational), rational(value) {}
        explicit Node(Kind k) : kind(k) {}
    };

    std::shared_ptr<const Node> node;

    explicit ExactReal(std::shared_ptr<const Node> value) : node(std::move(value)) {}

    static ExactReal binary(Kind kind, const ExactReal& a, const ExactReal& b)
    {
        auto n = std::make_shared<Node>(kind);
        n->left = a.node;
        n->right = b.node;
        return ExactReal(n);
    }

    static std::pair<Fraction<BigInteger>,std::shared_ptr<const Node>> coefficient(const std::shared_ptr<const Node>& n)
    {
        if(n->kind==Kind::Rational) return {n->rational,{}};
        if(n->kind==Kind::Multiply)
        {
            if(n->left->kind==Kind::Rational) { auto c=coefficient(n->right); c.first*=n->left->rational; return c; }
            if(n->right->kind==Kind::Rational) { auto c=coefficient(n->left); c.first*=n->right->rational; return c; }
        }
        return {Fraction<BigInteger>(1),n};
    }

    static ExactReal scaled(const Fraction<BigInteger>& c,const std::shared_ptr<const Node>& base)
    {
        if(!base || c==Fraction<BigInteger>(0)) return ExactReal(c);
        if(c==Fraction<BigInteger>(1)) return ExactReal(base);
        auto n=std::make_shared<Node>(Kind::Multiply);
        n->left=ExactReal(c).node; n->right=base;
        return ExactReal(n);
    }

    static BigInteger integerPower(BigInteger base, BigInteger exponent)
    {
        BigInteger result = 1;
        while (exponent > 0)
        {
            if ((exponent & 1) != 0) result *= base;
            exponent >>= 1;
            if (exponent > 0) base *= base;
        }
        return result;
    }

    static bool same(const std::shared_ptr<const Node>& a,
                     const std::shared_ptr<const Node>& b)
    {
        if (a == b) return true;
        if (!a || !b || a->kind != b->kind) return false;
        if (a->kind == Kind::Rational) return a->rational == b->rational;
        if (a->kind == Kind::Pi || a->kind == Kind::Euler) return true;
        if (a->exponent != b->exponent || a->degree != b->degree) return false;
        if (a->kind == Kind::Add || a->kind == Kind::Multiply)
            return (same(a->left,b->left) && same(a->right,b->right))
                || (same(a->left,b->right) && same(a->right,b->left));
        return same(a->left, b->left) && same(a->right, b->right);
    }

    static std::string key(const std::shared_ptr<const Node>& n)
    {
        if(!n) return "#";
        if(n->kind==Kind::Add || n->kind==Kind::Multiply)
        {
            std::vector<std::string> terms;
            auto gather=[&](auto&& self,const std::shared_ptr<const Node>& p)->void
            { if(p->kind==n->kind){self(self,p->left);self(self,p->right);} else terms.push_back(key(p)); };
            gather(gather,n); std::sort(terms.begin(),terms.end());
            std::string result=n->kind==Kind::Add?"A[":"M[";
            for(const auto& t:terms) result+=t+";";
            return result+"]";
        }
        if(n->kind==Kind::Rational) { std::ostringstream s; s<<n->rational; return "Q"+s.str(); }
        if(n->kind==Kind::Pi) return "pi";
        if(n->kind==Kind::Euler) return "e";
        return std::to_string(static_cast<int>(n->kind))+"("+key(n->left)+","+key(n->right)+","+std::to_string(n->exponent)+","+n->degree.convert_to<std::string>()+")";
    }

    static std::string render(const std::shared_ptr<const Node>& n)
    {
        switch (n->kind)
        {
            case Kind::Rational: { std::ostringstream s; s<<n->rational; return s.str(); }
            case Kind::Pi: return "pi";
            case Kind::Euler: return "e";
            case Kind::Add: return "(" + render(n->left) + " + " + render(n->right) + ")";
            case Kind::Subtract: return "(" + render(n->left) + " - " + render(n->right) + ")";
            case Kind::Multiply:
                if(n->left->kind==Kind::Rational && n->right->kind==Kind::Root)
                { std::ostringstream c; c<<n->left->rational; return (n->left->rational==Fraction<BigInteger>(1)?"":c.str())+render(n->right); }
                if(n->right->kind==Kind::Rational && n->left->kind==Kind::Root)
                { std::ostringstream c; c<<n->right->rational; return (n->right->rational==Fraction<BigInteger>(1)?"":c.str())+render(n->left); }
                return "(" + render(n->left) + " * " + render(n->right) + ")";
            case Kind::Divide: return "(" + render(n->left) + " / " + render(n->right) + ")";
            case Kind::Power: return "(" + render(n->left) + "^" + std::to_string(n->exponent) + ")";
            case Kind::Root: return n->degree==2 ? "sqrt("+render(n->left)+")" : "root(" + n->degree.convert_to<std::string>() + "," + render(n->left) + ")";
            case Kind::Exp: return "exp(" + render(n->left) + ")";
            case Kind::Log: return "log(" + render(n->left) + ")";
            case Kind::Sin: return "sin(" + render(n->left) + ")";
            case Kind::Cos: return "cos(" + render(n->left) + ")";
            case Kind::Atan2: return "atan2(" + render(n->left) + "," + render(n->right) + ")";
        }
        throw std::logic_error("Unknown exact-real expression.");
    }

    static double evaluate(const std::shared_ptr<const Node>& n)
    {
        switch (n->kind)
        {
            case Kind::Rational: return n->rational.aprox();
            case Kind::Pi: return std::numbers::pi_v<double>;
            case Kind::Euler: return std::exp(1.0);
            case Kind::Add: return evaluate(n->left) + evaluate(n->right);
            case Kind::Subtract: return evaluate(n->left) - evaluate(n->right);
            case Kind::Multiply: return evaluate(n->left) * evaluate(n->right);
            case Kind::Divide:
            {
                const double d = evaluate(n->right);
                if (d == 0.0) throw std::invalid_argument("Division by zero.");
                return evaluate(n->left) / d;
            }
            case Kind::Power: return std::pow(evaluate(n->left), n->exponent);
            case Kind::Root: { const double x=evaluate(n->left); const double d=n->degree.convert_to<double>(); return x<0 && (n->degree%2!=0) ? -std::pow(-x,1.0/d) : std::pow(x,1.0/d); }
            case Kind::Exp: return std::exp(evaluate(n->left));
            case Kind::Log: return std::log(evaluate(n->left));
            case Kind::Sin: return std::sin(evaluate(n->left));
            case Kind::Cos: return std::cos(evaluate(n->left));
            case Kind::Atan2: return std::atan2(evaluate(n->left), evaluate(n->right));
        }
        throw std::logic_error("Unknown exact-real expression.");
    }

    static Decimal evaluate100(const std::shared_ptr<const Node>& n)
    {
        switch (n->kind)
        {
            case Kind::Rational: return Decimal(n->rational.getNumerator().convert_to<std::string>())/Decimal(n->rational.getDenominator().convert_to<std::string>());
            case Kind::Pi: return boost::math::constants::pi<Decimal>();
            case Kind::Euler: return boost::multiprecision::exp(Decimal(1));
            case Kind::Add: return evaluate100(n->left) + evaluate100(n->right);
            case Kind::Subtract: return evaluate100(n->left) - evaluate100(n->right);
            case Kind::Multiply: return evaluate100(n->left) * evaluate100(n->right);
            case Kind::Divide:
            {
                Decimal d=evaluate100(n->right); if(d==0) throw std::invalid_argument("Division by zero.");
                return evaluate100(n->left)/d;
            }
            case Kind::Power: return pow(evaluate100(n->left), n->exponent);
            case Kind::Root: { const Decimal x=evaluate100(n->left), d(n->degree.convert_to<std::string>()); return x<0 && (n->degree%2!=0) ? -pow(-x,Decimal(1)/d) : pow(x,Decimal(1)/d); }
            case Kind::Exp: return boost::multiprecision::exp(evaluate100(n->left));
            case Kind::Log: return boost::multiprecision::log(evaluate100(n->left));
            case Kind::Sin: return boost::multiprecision::sin(evaluate100(n->left));
            case Kind::Cos: return boost::multiprecision::cos(evaluate100(n->left));
            case Kind::Atan2: return boost::multiprecision::atan2(evaluate100(n->left),evaluate100(n->right));
        }
        throw std::logic_error("Unknown exact-real expression.");
    }

    bool isRationalZero() const
    { return node->kind == Kind::Rational && node->rational == Fraction<BigInteger>(0); }
    bool isRationalOne() const
    { return node->kind == Kind::Rational && node->rational == Fraction<BigInteger>(1); }

    static ExactReal fromIrrational(const IrrationalNumber& value)
    {
        ExactReal result(value.getRationalPart());
        for(const auto& term:value.getRadicalTerms())
        {
            auto rootNode=std::make_shared<Node>(Kind::Root);
            rootNode->left=ExactReal(term.radicand).node;
            rootNode->degree=term.degree;
            result=result+ExactReal(term.coefficient)*ExactReal(rootNode);
        }
        return result;
    }

public:
    ExactReal() : node(std::make_shared<Node>(Fraction<BigInteger>(0))) {}
    ExactReal(long long value) : node(std::make_shared<Node>(Fraction<BigInteger>(BigInteger(value)))) {}
    ExactReal(const BigInteger& value) : node(std::make_shared<Node>(Fraction<BigInteger>(value))) {}
    ExactReal(const IrrationalNumber& value) : ExactReal(fromIrrational(value)) {}
    ExactReal(const RationalNumber& value) : node(std::make_shared<Node>(value.getFraction())) {}
    ExactReal(const RealNumber& value)
    {
        if (const auto* exact = dynamic_cast<const ExactReal*>(&value)) node = exact->node;
        else if (const auto* rational = dynamic_cast<const RationalNumber*>(&value))
            node = std::make_shared<Node>(rational->getFraction());
        else if (const auto* irrational = dynamic_cast<const IrrationalNumber*>(&value))
            node = fromIrrational(*irrational).node;
        else throw std::invalid_argument("Unsupported real-number implementation.");
    }

    template <FractionType T>
    ExactReal(const Fraction<T>& value) : node(std::make_shared<Node>(Fraction<BigInteger>(value))) {}

    static ExactReal pi() { return ExactReal(std::make_shared<Node>(Kind::Pi)); }
    static ExactReal e() { return ExactReal(std::make_shared<Node>(Kind::Euler)); }
    static ExactReal exp(const ExactReal& x) { auto n=std::make_shared<Node>(Kind::Exp); n->left=x.node; return ExactReal(n); }
    static ExactReal log(const ExactReal& x) { if(x.aprox100()<=0) throw std::invalid_argument("Real logarithm requires a positive input."); auto n=std::make_shared<Node>(Kind::Log); n->left=x.node; return ExactReal(n); }
    static ExactReal sin(const ExactReal& x) { auto n=std::make_shared<Node>(Kind::Sin); n->left=x.node; return ExactReal(n); }
    static ExactReal cos(const ExactReal& x) { auto n=std::make_shared<Node>(Kind::Cos); n->left=x.node; return ExactReal(n); }
    static ExactReal atan2(const ExactReal& y,const ExactReal& x) { auto n=std::make_shared<Node>(Kind::Atan2); n->left=y.node; n->right=x.node; return ExactReal(n); }

    static ExactReal root(const BigInteger& degree, const ExactReal& radicand)
    {
        if (degree < 2) throw std::invalid_argument("Root degree must be at least 2.");
        if (radicand.isRationalZero()) return ExactReal(0);
        if (radicand.node->kind == Kind::Rational)
        {
            const auto& f = radicand.node->rational;
            if (f.getNumerator() < 0 && degree % 2 == 0) throw std::invalid_argument("Even real root requires a nonnegative radicand.");
            const bool negative=f.getNumerator()<0;
            const BigInteger n = (negative ? -f.getNumerator() : f.getNumerator())
                               * integerPower(f.getDenominator(), degree - 1);
            const BigInteger d = f.getDenominator();
            return fromIrrational(IrrationalNumber(Fraction<BigInteger>(negative ? -1 : 1, d), degree, n));
        }
        auto n = std::make_shared<Node>(Kind::Root);
        n->left = radicand.node;
        n->degree = degree;
        return ExactReal(n);
    }

    bool isZero() const { return isRationalZero(); }
    std::optional<BigInteger> integerValue() const
    {
        if(node->kind!=Kind::Rational || node->rational.getDenominator()!=1) return std::nullopt;
        return node->rational.getNumerator();
    }
    double aprox() const override { return evaluate(node); }
    Decimal aprox100() const override { return evaluate100(node); }
    std::string toString() const override { return render(node); }

    ExactReal operator+(const ExactReal& other) const
    {
        if (isRationalZero()) return other;
        if (other.isRationalZero()) return *this;
        if (node->kind == Kind::Rational && other.node->kind == Kind::Rational)
            return ExactReal(node->rational + other.node->rational);
        const auto a=coefficient(node), b=coefficient(other.node);
        if(a.second && b.second && key(a.second)==key(b.second))
            return scaled(a.first+b.first,a.second);
        return binary(Kind::Add, *this, other);
    }

    ExactReal operator-(const ExactReal& other) const
    {
        if (*this == other) return ExactReal(0);
        if (other.isRationalZero()) return *this;
        if (node->kind == Kind::Rational && other.node->kind == Kind::Rational)
            return ExactReal(node->rational - other.node->rational);
        const auto a=coefficient(node), b=coefficient(other.node);
        if(a.second && b.second && key(a.second)==key(b.second))
            return scaled(a.first-b.first,a.second);
        return binary(Kind::Subtract, *this, other);
    }

    ExactReal operator*(const ExactReal& other) const
    {
        if (isRationalZero() || other.isRationalZero()) return ExactReal(0);
        if (isRationalOne()) return other;
        if (other.isRationalOne()) return *this;
        if (node->kind == Kind::Rational && other.node->kind == Kind::Rational)
            return ExactReal(node->rational * other.node->rational);
        if(node->kind==Kind::Rational || other.node->kind==Kind::Rational)
        {
            const auto& scalar=node->kind==Kind::Rational?node->rational:other.node->rational;
            const auto& term=node->kind==Kind::Rational?other.node:node;
            const auto c=coefficient(term);
            return scaled(scalar*c.first,c.second);
        }
        return binary(Kind::Multiply, *this, other);
    }

    ExactReal operator/(const ExactReal& other) const
    {
        if (other.isRationalZero()) throw std::invalid_argument("Division by zero.");
        if (*this == other) return ExactReal(1);
        if (isRationalZero()) return ExactReal(0);
        if (other.isRationalOne()) return *this;
        if (node->kind == Kind::Rational && other.node->kind == Kind::Rational)
            return ExactReal(node->rational / other.node->rational);
        return binary(Kind::Divide, *this, other);
    }

    ExactReal operator^(int exponent) const
    {
        if (exponent == 0) return ExactReal(1);
        if (exponent == 1) return *this;
        if (isRationalZero() && exponent < 0) throw std::invalid_argument("Zero cannot have a negative exponent.");
        if (node->kind == Kind::Rational) return ExactReal(node->rational ^ exponent);
        auto n = std::make_shared<Node>(Kind::Power);
        n->left = node; n->exponent = exponent;
        return ExactReal(n);
    }

    bool operator==(const ExactReal& other) const { return key(node)==key(other.node); }
    bool operator!=(const ExactReal& other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream& out, const ExactReal& value)
    { return out << value.toString(); }
};

inline ExactReal operator+(const RealNumber& lhs, const ExactReal& rhs)
{ return ExactReal(lhs) + rhs; }
inline ExactReal operator-(const RealNumber& lhs, const ExactReal& rhs)
{ return ExactReal(lhs) - rhs; }
inline ExactReal operator*(const RealNumber& lhs, const ExactReal& rhs)
{ return ExactReal(lhs) * rhs; }
inline ExactReal operator/(const RealNumber& lhs, const ExactReal& rhs)
{ return ExactReal(lhs) / rhs; }
