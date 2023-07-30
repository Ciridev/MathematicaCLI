#pragma once

#include "Core/Utility/Utils.h"
#include "Core/Utility/Types.h"

class Hashable
{
public:
	Hashable(); 
	uint32 GetHash() const { return mHash; }

	virtual void Rehash() = 0;

	static bool Compare(const Hashable& first, const Hashable& second);

protected:
	template<typename T>
	void HashField(T field);

private:
	void Hash(uint32 seed);

	uint32 mHash;
};

template<typename T>
inline void Hashable::HashField(T field)
{
	MTH_ASSERT(false, "HashError: To use Hashable with this type, you have to implement your own hash function!");
}

template<>
inline void Hashable::HashField(uint32 field)
{
	Hash(field);
}

template<>
inline void Hashable::HashField(String field)
{
	uint32 hashNumber = 0;

	const uint16 magicHigh = 0xaf13;
	const uint16 magicLow = 0x4b71;

	for (auto c : field) hashNumber += c * magicHigh - magicLow / c;

	Hash(hashNumber);
}