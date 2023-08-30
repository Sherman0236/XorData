#pragma once
#include "FnvHash.hpp"
#include <array>
#include <string>
#include <immintrin.h>

#define MakeXorString_(str) MakeXorString(str).GetCrypt()
#define MakeXorString(str)																					\
    []()																									\
	{																										\
        using CharType = std::remove_const_t<std::remove_reference_t<decltype(str[0])>>;					\
        constexpr size_t length = sizeof(str) / sizeof(CharType) - 1;										\
        constexpr CharType key = hash::fnv<UNIQUE_SEED>(str) & ((1ull << (sizeof(CharType) * 8)) - 1);	\
        constexpr auto result = enc::XorString<CharType, length, key>(str);									\
		return result;																						\
    }()

namespace enc
{
	/// <summary>
	/// Represents a string, which is encrypted at compile-time
	/// </summary>
	/// <typeparam name="Length">The length of the string</typeparam>
	/// <typeparam name="Key">A compile-time key used for XOR operations</typeparam>
	/// <typeparam name="CharType">The character type of the string</typeparam>
	template <typename CharType, size_t Length, CharType Key>
	class XorString
	{
	public:
		/// <summary>
		/// Initializes a XorString class
		/// </summary>
		/// <param name="string">The raw string to be encrypted</param>
		constexpr XorString(const CharType* string) 
		{
			Encrypt(string);
		}

		/// <summary>
		/// Decrypts and returns the stored string
		/// </summary>
		/// <returns>The decrypted string</returns>
		__forceinline auto GetCrypt()
		{
			return Decrypt();
		}

	private:
		/// <summary>
		/// Stores and encrypts the specified string
		/// </summary>
		/// <param name="string">String to be encrypted</param>
		constexpr void Encrypt(const CharType* string)
		{
			for (size_t i = 0; i < Length; i++)
			{
				m_encryptedData[i] = string[i] ^ (Key + (i % 13));
			}
		}

		/// <summary>
		/// Decrypts the stored string
		/// </summary>
		/// <returns>The decrypted string</returns>
		__forceinline std::basic_string<CharType> Decrypt()
		{
			std::basic_string<CharType> result = {};
			result.reserve(Length);
			
			for (size_t i = 0; i < m_encryptedData.size(); i++)
			{
				__m128i decrypted = _mm_xor_si128(_mm_set1_epi16(m_encryptedData[i]), _mm_set1_epi16(Key + (i % 13)));
				result.push_back(static_cast<CharType>(_mm_extract_epi16(decrypted, 0)));
			}

			return result;
		}

		// Encrypted characters stored as an array of 64-bit values
		std::array<CharType, Length> m_encryptedData = {};
	};
}