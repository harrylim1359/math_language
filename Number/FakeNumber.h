#pragma once

#include "Number.h"
#include "RealNumber.h"

#include <memory>
#include <iostream>
#include <string>
#include <cmath>
#include <utility>
#include <stdexcept>

class FakeNumber : public Number
{
private:
    std::shared_ptr<RealNumber> realPart;
    std::shared_ptr<RealNumber> imaginaryPart;

public:

    FakeNumber(
        std::shared_ptr<RealNumber> real,
        std::shared_ptr<RealNumber> imaginary
    )
        : realPart(std::move(real)),
          imaginaryPart(std::move(imaginary))
    {
        if (!realPart || !imaginaryPart)
        {
            throw std::invalid_argument(
                "FakeNumber parts cannot be null."
            );
        }
    }


    const RealNumber& getRealPart() const
    {
        return *realPart;
    }


    const RealNumber& getImaginaryPart() const
    {
        return *imaginaryPart;
    }


    double aprox() const override
    {
        double real = realPart->aprox();
        double imaginary = imaginaryPart->aprox();

        return std::sqrt(
            real * real
            + imaginary * imaginary
        );
    }


    std::string toString() const override
    {
        return realPart->toString()
             + " + "
             + imaginaryPart->toString()
             + "i";
    }


    friend std::ostream& operator<<(
        std::ostream& os,
        const FakeNumber& number
    )
    {
        os << number.toString();

        return os;
    }
};
