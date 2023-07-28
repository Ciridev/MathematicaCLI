#include "mthpch.h"

#include "Core/Utility/Profiler.h"
#include "Core/Utility/Utils.h"

#include "Core/Math/Integer.h"
#include "Core/Math/Number.h"

namespace Mathematica
{
    namespace Integer
    {
        int32 LeastCommonMultiple(int32 a, int32 b)
        {   
            MTH_PROFILE_FUNCTION();

            return a * b / GreatestCommonDivisor(a, b);
        }

        int32 GreatestCommonDivisor(int32 a, int32 b)
        {
            MTH_PROFILE_FUNCTION();

            if(a == 0 || b == 0) return a == 0 ? b : a;
            if(a == b) return a;

            int32 greater = Mathematica::Max(a, b);
            int32 lower = Mathematica::Min(a, b);
            int32 remainder = greater % lower;

            while (remainder != 0)
            {
                greater = lower;
                lower = remainder;
                remainder = greater % lower;
            }
            
            return lower;
        }

		bool IsPrime(int32 n)
		{
            MTH_PROFILE_FUNCTION();

            if (n < 2) return false;

            for (int32 i = 2; i * i <= n; i++)
            {
                if (n % i == 0) return false;
            }

            return true;
		}

        bool IsPrime(Number n)
        {
            MTH_PROFILE_FUNCTION();

			MTH_ASSERT(n.type == ENumberType::Integer, "DomainError: Cannot factorize a non-integer number!");
			return IsPrime(n.numerator);
        }

        Map<int32, int32> Factorize(int32 n)
        {
            MTH_PROFILE_FUNCTION();

            auto soe = SoE(n);
            Map<int32, int32> result;

			for (int32 i = 0; i < soe.size() && n > 1; i++)
            {
                int32 currentPrime = soe[i];

                if (n % currentPrime == 0)
                {
                    if (result.find(currentPrime) != result.end())
                    {
                        result[currentPrime]++;
                    }
                    else
                    {
                        result[currentPrime] = 1;
                    }

                    n /= currentPrime;
                    i--;
                }
            }

            return result;
        }

        Map<int32, int32> Factorize(Number n)
        {
            MTH_ASSERT(n.type == ENumberType::Integer, "DomainError: Cannot factorize a non-integer number!");
            return Factorize(n.numerator);
        }

        Vector<int32> SoE(int32 max)
        {
            MTH_PROFILE_FUNCTION();

            // TODO : Implement a proper SoE and speed up the algorithm.
            Vector<int32> result;

            for (int32 i = 2; i <= max; i++)
            {
                if (IsPrime(i))
                {
                    result.push_back(i);
                }
            }

            return result;
        }
	}
}
