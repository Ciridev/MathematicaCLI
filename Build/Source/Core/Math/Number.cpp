#include "mthpch.h"

#include "Core/Math/Operations.h"
#include "Core/Math/Rational.h"
#include "Core/Math/Integer.h"
#include "Core/Math/Number.h"

#include "Core/MathNode.h"

#include "Core/Utility/Conversions.h"
#include "Core/Utility/Profiler.h"
#include "Core/Utility/Utils.h"

RationalNumber::RationalNumber(Int32 num, Int32 den) : numerator(num), denominator(den), type(ENumberType::Rational)
{
    MTH_ASSERT(denominator != 0, "NumberInitError: Cannot divide by zero!");

    LowestTerms();

    if (denominator == 1)
    {
        type = ENumberType::Integer;
    }
}

RationalNumber::RationalNumber(const String& strNumber)
{
    // TODO : Add support for fractions and real numbers.
    Float32 rawNumerical = Mathematica::Convert::StringToFloat32(strNumber);

    if (rawNumerical != Mathematica::Cast<Int32>(rawNumerical))
    {
        RationalNumber fraction = Mathematica::Rational::Farey(rawNumerical);
		numerator = fraction.numerator;
		denominator = fraction.denominator;
        type = ENumberType::Rational;
    }
    else
    {
        numerator = Mathematica::Cast<Int32>(rawNumerical);
        denominator = 1;
        type = ENumberType::Integer;
    }
}

Float32 RationalNumber::RawNumerical()
{
    return (Float32)numerator / (Float32)denominator;
}

RationalNumber RationalNumber::LowestTerms(Int32 numerator, Int32 denominator)
{
    Int32 gcd = Mathematica::Integer::GreatestCommonDivisor(numerator, denominator);
    return { numerator / gcd, denominator / gcd};
}

void RationalNumber::LowestTerms()
{
    Int32 gcd = Mathematica::Integer::GreatestCommonDivisor(numerator, denominator);
    
    numerator /= gcd;
    denominator /= gcd;
}

RationalNumber RationalNumber::operator+(RationalNumber other)
{
    return Mathematica::Operation::Add(*this, other);
}

RationalNumber RationalNumber::operator-(RationalNumber other)
{
    return Mathematica::Operation::Subtract(*this, other);
}

RationalNumber RationalNumber::operator*(RationalNumber other)
{
    return Mathematica::Operation::Multiply(*this, other);
}

RationalNumber RationalNumber::operator/(RationalNumber other)
{
    return Mathematica::Operation::Divide(*this, other);
}

const RationalNumber& RationalNumber::operator+(RationalNumber other) const
{
    return Mathematica::Operation::Add(*this, other);
}

const RationalNumber& RationalNumber::operator-(RationalNumber other) const
{
    return Mathematica::Operation::Subtract(*this, other);
}

const RationalNumber& RationalNumber::operator*(RationalNumber other) const
{
    return Mathematica::Operation::Multiply(*this, other);
}

const RationalNumber& RationalNumber::operator/(RationalNumber other) const
{
    return Mathematica::Operation::Divide(*this, other);
}

void RationalNumber::operator+=(RationalNumber other)
{
    *this = Mathematica::Operation::Add(*this, other);
}

void RationalNumber::operator-=(RationalNumber other)
{
    *this = Mathematica::Operation::Subtract(*this, other);
}

void RationalNumber::operator*=(RationalNumber other)
{
    *this = Mathematica::Operation::Multiply(*this, other);
}

void RationalNumber::operator/=(RationalNumber other)
{
    *this = Mathematica::Operation::Divide(*this, other);
}

bool RationalNumber::operator==(RationalNumber other)
{
    return numerator * other.denominator == denominator * other.numerator;
}

bool RationalNumber::operator!=(RationalNumber other)
{
    return !(*this == other);
}

bool RationalNumber::operator>=(RationalNumber other)
{
    return numerator * other.denominator >= denominator * other.numerator;
}

bool RationalNumber::operator<=(RationalNumber other)
{
    return numerator * other.denominator <= denominator * other.numerator;
}

bool RationalNumber::operator>(RationalNumber other)
{
    return numerator * other.denominator > denominator * other.numerator;
}

bool RationalNumber::operator<(RationalNumber other)
{
    return numerator * other.denominator < denominator * other.numerator;
}

namespace Mathematica
{
    RationalNumber Absolute(RationalNumber number)
    {
        MTH_PROFILE_FUNCTION();

        return RationalNumber(number.numerator > 0 ? number.numerator : -number.numerator, number.denominator > 0 ? number.denominator : -number.denominator);
    }

    Int32 Sign(RationalNumber number)
    {
        MTH_PROFILE_FUNCTION();

        MTH_ASSERT(number != 0, "DomainError: Zero is not in the domain of Sign!");
        return number > 0 ? 1 : -1;
    }
}

// === Irrational Numbers ===

IrrationalNumber::IrrationalNumber(Ref<MathNode> num, Ref<MathNode> den)
    : numerator   (num), 
      denominator (den)
{
    if (numerator == Mathematica::MakeRef<MathNode>())
    {
		numerator->data = RationalNumber{};
		numerator->type = EMathNodeType::Number;
    }

	if (denominator == Mathematica::MakeRef<MathNode>())
	{
        denominator->data = RationalNumber{};
        denominator->type = EMathNodeType::Number;
	}
}

IrrationalNumber::IrrationalNumber(const String& constantName) 
    : numerator   (Mathematica::MakeRef<MathNode>()),
      denominator (Mathematica::MakeRef<MathNode>())
{
	numerator->data = RationalNumber{};
	numerator->type = EMathNodeType::Number;

    denominator->data = RationalNumber{};
    denominator->type = EMathNodeType::Number;
}

IrrationalNumber IrrationalNumber::operator+(IrrationalNumber other)
{
    return IrrationalNumber();
}

IrrationalNumber IrrationalNumber::operator-(IrrationalNumber other)
{
    return IrrationalNumber();
}

IrrationalNumber IrrationalNumber::operator*(IrrationalNumber other)
{
    return IrrationalNumber();
}

IrrationalNumber IrrationalNumber::operator/(IrrationalNumber other)
{
    return IrrationalNumber();
}

void IrrationalNumber::operator+=(IrrationalNumber other)
{
}

void IrrationalNumber::operator-=(IrrationalNumber other)
{
}

void IrrationalNumber::operator*=(IrrationalNumber other)
{
}

void IrrationalNumber::operator/=(IrrationalNumber other)
{
}

bool IrrationalNumber::operator==(IrrationalNumber other)
{
    return false;
}

bool IrrationalNumber::operator!=(IrrationalNumber other)
{
    return false;
}

bool IrrationalNumber::operator>=(IrrationalNumber other)
{
    return false;
}

bool IrrationalNumber::operator<=(IrrationalNumber other)
{
    return false;
}

bool IrrationalNumber::operator>(IrrationalNumber other)
{
    return false;
}

bool IrrationalNumber::operator<(IrrationalNumber other)
{
    return false;
}

void IrrationalNumber::Rehash()
{

}

Float32 IrrationalNumber::RawNumerical()
{
    return Float32();
}

RealNumber::RealNumber(RationalNumber rational, IrrationalPart irrational)
{
    rationalCoefficient = rational;
    irrationalCoefficients = irrational;

    if (rationalCoefficient == RationalNumber{})
    {
        type = ESubset::Irrational;
    }

    if (irrationalCoefficients.Size() == 0)
    {
        irrationalCoefficients.EmplaceBack(IrrationalNumber{});
        type = ESubset::Rational;
    }

    if (rationalCoefficient != RationalNumber{} && (irrationalCoefficients.Size() > 0/*&& irrationalCoefficients[0] != IrrationalNumber{})*/))
    {
        type = ESubset::Real;
	}
}

RealNumber::RealNumber(const String& strNumber)
{
}

RealNumber RealNumber::operator+(RealNumber other)
{
    return RealNumber();
}

RealNumber RealNumber::operator-(RealNumber other)
{
    return RealNumber();
}

RealNumber RealNumber::operator*(RealNumber other)
{
    return RealNumber();
}

RealNumber RealNumber::operator/(RealNumber other)
{
    return RealNumber();
}

void RealNumber::operator+=(RealNumber other)
{

}

void RealNumber::operator-=(RealNumber other)
{
}

void RealNumber::operator*=(RealNumber other)
{
}

void RealNumber::operator/=(RealNumber other)
{
}

bool RealNumber::operator==(RealNumber other)
{
    return rationalCoefficient == other.rationalCoefficient;
}

bool RealNumber::operator!=(RealNumber other)
{
    return false;
}

bool RealNumber::operator>=(RealNumber other)
{
    return false;
}

bool RealNumber::operator<=(RealNumber other)
{
    return false;
}

bool RealNumber::operator>(RealNumber other)
{
    return false;
}

bool RealNumber::operator<(RealNumber other)
{
    return false;
}

Float32 RealNumber::RawNumerical()
{
    return Float32();
}

void IrrationalPart::Rehash()
{
   for (auto& irrational : *this)
   {
        HashField(irrational);
   }
}

void IrrationalPart::PushBack(const IrrationalNumber& what)
{
    push_back(what);
    Rehash();
}

void IrrationalPart::PushBack(IrrationalNumber&& what)
{
	push_back(static_cast<IrrationalNumber&&>(what));
	Rehash();
}

void IrrationalPart::PopBack()
{
    pop_back();
    Rehash();
}
