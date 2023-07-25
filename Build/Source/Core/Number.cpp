#include "mthpch.h"

#include "Core/Math/Integer.h"
#include "Core/Math/Operations.h"

#include "Core/Number.h"

Number::Number(int32 num, int32 den) : numerator(num), denominator(den), type(ENumberType::Real) 
{
    // TODO : This implementation does not include Real type numbers. For now the only allowed types are Integer and Rational.
    if (denominator == 1)
    {
        type = ENumberType::Integer;
    }
    else
    {
        type = ENumberType::Rational;
    }
}

Number::Number(const String& strNumber)
{
    // TODO : Add support for fractions and real numbers.
    numerator = std::atoi(strNumber.c_str());
    denominator = 1;

    MTH_ASSERT(denominator != 0, "NumberInitError: Cannot divide by zero!");

    type = ENumberType::Integer;
}

double Number::RawNumerical()
{
    return (double)numerator / (double)denominator;
}

Number Number::LowestTerms()
{
    int32 gcd = Mathematica::Integer::GreatestCommonDivisor(numerator, denominator);
    return { numerator / gcd, denominator / gcd };
}

Number Number::operator+(Number other)
{
    return Mathematica::Operation::Add(*this, other);
}

Number Number::operator-(Number other)
{
    return Mathematica::Operation::Subtract(*this, other);
}

Number Number::operator*(Number other)
{
    return Mathematica::Operation::Multiply(*this, other);
}

Number Number::operator/(Number other)
{
    return Mathematica::Operation::Divide(*this, other);
}

void Number::operator+=(Number other)
{
    *this = Mathematica::Operation::Add(*this, other);
}

void Number::operator-=(Number other)
{
    *this = Mathematica::Operation::Subtract(*this, other);
}

void Number::operator*=(Number other)
{
    *this = Mathematica::Operation::Multiply(*this, other);
}

void Number::operator/=(Number other)
{
    *this = Mathematica::Operation::Divide(*this, other);
}
