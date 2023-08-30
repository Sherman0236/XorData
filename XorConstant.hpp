#pragma once
#include "FnvHash.hpp"
#include <type_traits>
#include <immintrin.h>

#define MakeXorConstant(value) enc::MakeXorConstantImpl<decltype(value), static_cast<uintptr_t>(value), hash::fnv<UNIQUE_SEED>(__FILE__)>()

namespace enc
{
	/// <summary>
	/// Represents a constant, which is encrypted at compile-time
	/// </summary>
	/// <typeparam name="T">Type of the constant</typeparam>
	/// <typeparam name="ConstantValue">Constant to be encrypted</typeparam>
	/// <typeparam name="Seed">The seed used for encryption</typeparam>
	template <typename T, uintptr_t ConstantValue, uintptr_t Seed>
	class XorConstant
	{
		static_assert(std::is_arithmetic<T>::value, "Type T must be either an integral or a floating-point type.");

	public:
		/// <summary>
		/// Initializes a XorConstant class with the given data
		/// </summary>
		constexpr XorConstant()
		{
			EncryptData(ConstantValue);
		}

		/// <summary>
		/// Gets the encrypted constant without decrypting it
		/// </summary>
		/// <returns>The encrypted constant</returns>
		__forceinline T Get()
		{
#ifdef _WIN64
			return static_cast<T>(_mm_cvtsi128_si64(m_encryptedData));
#else
			return static_cast<T>(_mm_cvtsi128_si32(m_encryptedData));
#endif
		}

		/// <summary>
		/// Gets the decrypted constant
		/// </summary>
		/// <returns>The decrypted constant</returns>
		__forceinline T GetCrypt()
		{
			return static_cast<T>(DecryptData());
		}

	private:
		/// <summary>
		/// Encrypts and stores the provided data
		/// </summary>
		constexpr void EncryptData(uintptr_t data)
		{
			__m128i result = _mm_set1_epi64x(data);
			result = _mm_xor_si128(result, _mm_set1_epi64x(XorKey));
			result = _mm_add_epi64(result, _mm_set1_epi64x(AddKey));
			m_encryptedData = result;
		}

		/// <summary>
		/// Decrypts the stored constant
		/// </summary>
		/// <returns>The decrypted constant</returns>
		/// <remarks>
		/// '__declspec(noinline)' prevents the compiler from over-optimizing 
		/// the function and revealing the constant.
		/// </remarks>
		__declspec(noinline) uintptr_t DecryptData()
		{
			__m128i result = m_encryptedData;
			result = _mm_sub_epi64(result, _mm_set1_epi64x(AddKey));
			result = _mm_xor_si128(result, _mm_set1_epi64x(XorKey));
#ifdef _WIN64
			return _mm_cvtsi128_si64(result);
#else
			return _mm_cvtsi128_si32(result);
#endif
		}

		// Encrypted data member
		__m128i m_encryptedData = {};

		// Compile-time constants used for encryption and decryption.
		// The values are determined based on the seed, 
		// ensuring unique keys for each type and seed combination.		
		constexpr static uintptr_t XorKey = hash::key<Seed - __LINE__>();
		constexpr static uintptr_t AddKey = hash::key<Seed + __LINE__>();
	};

	/// <summary>
	/// A utility function to create an encrypted constant
	/// </summary>
	/// <typeparam name="T">The data type of the constant</typeparam>
	/// <typeparam name="Seed">The seed used for encryption</typeparam>
	/// <typeparam name="ConstantValue">The constant to be encrypted</typeparam>
	/// <returns>An instance of XorConstant</returns>
	template <typename T, uintptr_t ConstantValue, uintptr_t Seed>
	constexpr inline auto MakeXorConstantImpl()
	{
		return XorConstant<T, ConstantValue, Seed>();
	}
}