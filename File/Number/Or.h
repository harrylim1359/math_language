#pragma once

#include "BigInteger.h"
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

inline BigInteger encodeSignedIndex(const BigInteger& value)
{ if(value>=0) return BigInteger(2*value); return BigInteger(-2*value-1); }
inline BigInteger decodeSignedIndex(const BigInteger& value)
{ if(value%2==0) return BigInteger(value/2); return BigInteger(-(value+1)/2); }

// A finite set of alternatives or an unbounded, lazily indexed family.
template<class T>
class Or
{
    std::vector<T> values;
    std::function<T(const BigInteger&)> family;
    bool infinite = false;
public:
    Or() = default;
    explicit Or(std::vector<T> finiteValues) : values(std::move(finiteValues)) {}
    static Or lazy(std::function<T(const BigInteger&)> get)
    {
        Or result; result.family=std::move(get); result.infinite=true; return result;
    }
    bool isInfinite() const { return infinite; }
    std::size_t size() const
    { if(infinite) throw std::logic_error("An infinite Or has no finite size."); return values.size(); }
    const std::vector<T>& finiteValues() const
    { if(infinite) throw std::logic_error("An infinite Or has no finite value list."); return values; }
    T at(std::size_t index) const
    { if(infinite) throw std::logic_error("Use branch() for an infinite Or."); return values.at(index); }
    T branch(const BigInteger& index) const
    {
        if(infinite) return family(index);
        if(index<0) throw std::out_of_range("Finite branch index must be nonnegative.");
        if(index>=values.size()) throw std::out_of_range("Or branch index out of range.");
        return values[index.convert_to<std::size_t>()];
    }
};

template<class T,class F>
Or<T> orCombine(const Or<T>& a,const Or<T>& b,F operation)
{
    if(!a.isInfinite() && !b.isInfinite())
    {
        std::vector<T> out;
        for(const auto& x:a.finiteValues()) for(const auto& y:b.finiteValues()) out.push_back(operation(x,y));
        return Or<T>(std::move(out));
    }
    // Diagonal enumeration keeps the Cartesian product of infinite families lazy.
    if(!a.isInfinite())
        return Or<T>::lazy([a,b,operation](const BigInteger& index)
        { const auto n=a.size(); if(!n) throw std::out_of_range("Empty alternative set."); const BigInteger ordinal=encodeSignedIndex(index); BigInteger slot=ordinal%n, family=ordinal/n; return operation(a.at(slot.convert_to<std::size_t>()),b.branch(decodeSignedIndex(family))); });
    if(!b.isInfinite())
        return Or<T>::lazy([a,b,operation](const BigInteger& index)
        { const auto n=b.size(); if(!n) throw std::out_of_range("Empty alternative set."); const BigInteger ordinal=encodeSignedIndex(index); BigInteger slot=ordinal%n, family=ordinal/n; return operation(a.branch(decodeSignedIndex(family)),b.at(slot.convert_to<std::size_t>())); });
    return Or<T>::lazy([a,b,operation](const BigInteger& index)
    {
        const BigInteger ordinal=encodeSignedIndex(index);
        BigInteger diagonal=0,offset=ordinal;
        while(offset>diagonal){offset-=diagonal+1;++diagonal;}
        const BigInteger i=offset,j=diagonal-offset;
        const BigInteger branchI=decodeSignedIndex(i);
        const BigInteger branchJ=decodeSignedIndex(j);
        const T x=a.branch(branchI), y=b.branch(branchJ);
        return operation(x,y);
    });
}
