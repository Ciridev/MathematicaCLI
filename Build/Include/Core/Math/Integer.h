#pragma once

#include "Core/Utility/Utils.h"

namespace Mathematica
{
    namespace Integer
    {
        // Integer functions
        int32 LeastCommonMultiple(int32 a, int32 b);
        int32 GreatestCommonDivisor(int32 a, int32 b);

        // Prime numbers
        Map<int32, int32> Factorize(int32 n);
        Map<int32, int32> Factorize(Number n);

        bool IsPrime(int32 n);
        bool IsPrime(Number n);

        // Sieve of Erathosthenes
        Vector<int32> SoE (int32 max); 
    }
}