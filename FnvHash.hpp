#pragma once
#include <cstdint>
#include <string_view>

// https://stackoverflow.com/questions/57137351/line-is-not-constexpr-in-msvc
#define CAT(X,Y) CAT2(X,Y)
#define CAT2(X,Y) X##Y
#define USABLE_LINE uint32_t(CAT(__LINE__,U)) 

// generates a 64-bit seed based on the line number, counter, file name, and compile time
#define UNIQUE_SEED64 hash::key64<hash::fnv64<USABLE_LINE+__COUNTER__>(__FILE__)>()

namespace hash
{
	// FNV constants for 64-bit hash
	constexpr uint64_t FNV_PRIME_64 = 1099511628211;
	constexpr uint64_t FNV_BASIS_64 = 14695981039346656037;

	/// <summary>
	/// Calculates the 64-bit FNV-1a hash of a given byte or wide string
	/// </summary>
	/// <typeparam name="CharType">The character type of the string</typeparam>
	/// <param name="data">The string to hash</param>
	/// <returns>64-bit hash value</returns>
	template <uint64_t Seed, typename CharType> 
	constexpr uint64_t fnv64(std::basic_string_view<CharType> data)
	{
		uint64_t result = FNV_BASIS_64 ^ Seed;
		for (CharType c : data)
		{
			result ^= static_cast<uint64_t>(c);
			result *= FNV_PRIME_64;
		}
		return result;
	}

	/// <summary>
	/// Calculates the 64-bit FNV-1a hash of a given byte string
	/// </summary>
	/// <param name="data">String to hash</param>
	/// <returns>64-bit hash value</returns>
	template <uint64_t Seed>
	constexpr uint64_t fnv64(std::string_view data)
	{
		return fnv64<Seed, char>(data);
	}

	/// <summary>
	/// Calculates the 64-bit FNV-1a hash of a given wide string
	/// </summary>
	/// <param name="data">String to hash</param>
	/// <returns>64-bit hash value</returns>
	template <uint64_t Seed>
	constexpr uint64_t fnv64(std::wstring_view data)
	{
		return fnv64<Seed, wchar_t>(data);
	}

	/// <summary>
	/// Produces a 64-bit FNV-1a hash value that is semi-unique for each compilation
	/// </summary>
	/// <returns>64-bit hash value</returns>
	template <uint64_t Seed>
	constexpr uint64_t key64()
	{
		return fnv64<Seed>(__TIME__);
	}
}