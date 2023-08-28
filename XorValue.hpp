#pragma once
#include "XorConstant.hpp"
#include <immintrin.h>

#define MakeXorVariable(value) enc::MakeXorVariableImpl<decltype(value), hash::fnv64<UNIQUE_SEED64>(__FILE__)>(value)

namespace enc
{
	/// <summary>
	/// Represents a modifiable value, which is encrypted at run-time
	/// </summary>
	/// <typeparam name="T">Type of the value</typeparam>
	/// <typeparam name="Seed">The seed used for key generation</typeparam>
	template <typename T, uint64_t Seed>
	class XorValue
	{
		static_assert(std::is_arithmetic<T>::value || std::is_pointer<T>::value, 
			"Type T must be an integral type, a pointer, or a floating point type.");

		template<typename...>
		static constexpr bool always_false_v = false;
	public:
		/// <summary>
		/// Initializes a XorValue class with the given data
		/// </summary>
		constexpr XorValue(T data)
		{
			Encrypt(data);
		}

		/// <summary>
		/// Decrypts and returns the stored value
		/// </summary>
		/// <returns>The decrypted value</returns>
		__forceinline T GetCrypt()
		{
			return Decrypt();
		}

		/// <summary>
		/// Overload arrow operator to decrypt the stored pointer
		/// </summary>
		/// <returns>The decrypted pointer</returns>
		__forceinline T operator->()
		{
			static_assert(std::is_pointer_v<T>, "Type T must be a pointer to use operator->.");
			return Decrypt();
		}

		/// <summary>
		/// Overload assignment operator to encrypt the provided data
		/// </summary>
		/// <param name="data">The value to encrypt and store</param>
		__forceinline void operator=(T data)
		{
			Encrypt(data);
		}

	private:
		/// <summary>
		/// Encrypts and stores the provided provided data
		/// </summary>
		/// <param name="data">The value to encrypt</param>
		constexpr __forceinline void Encrypt(T data)
		{
			__m128i result = Encode(data);
			result = _mm_add_epi64(result, _mm_set1_epi64x(Key1.GetCrypt()));
			result = _mm_xor_si128(result, _mm_set1_epi64x(Key1.Get()));
			result = _mm_sub_epi64(result, _mm_set1_epi64x(Key2.GetCrypt()));
			result = _mm_xor_si128(result, _mm_set1_epi64x(Key2.Get()));
			result = _mm_add_epi64(result, _mm_set1_epi64x(Key3.GetCrypt()));
			result = _mm_xor_si128(result, _mm_set1_epi64x(Key3.Get()));
			m_encryptedData = result;
		}

		/// <summary>
		/// Decrypts the stored value
		/// </summary>
		/// <returns>The decrypted value</returns>
		__forceinline T Decrypt()
		{
			__m128i data = m_encryptedData;
			data = _mm_xor_si128(data, _mm_set1_epi64x(Key3.Get()));
			data = _mm_sub_epi64(data, _mm_set1_epi64x(Key3.GetCrypt()));
			data = _mm_xor_si128(data, _mm_set1_epi64x(Key2.Get()));
			data = _mm_add_epi64(data, _mm_set1_epi64x(Key2.GetCrypt()));
			data = _mm_xor_si128(data, _mm_set1_epi64x(Key1.Get()));
			data = _mm_sub_epi64(data, _mm_set1_epi64x(Key1.GetCrypt()));
			return Decode(data);
		}

		/// <summary>
		/// Converts the provided value into an __m128i format
		/// </summary>
		/// <param name="data">The value to encode</param>
		/// <returns>The __m128i representation of the value</returns>
		constexpr __forceinline __m128i Encode(T data)
		{
			if constexpr (std::is_pointer_v<T>)
			{
				return _mm_set1_epi64x(reinterpret_cast<uint64_t>(data));
			}
			
			if constexpr (std::is_integral_v<T>)
			{
				return _mm_set1_epi64x(static_cast<uint64_t>(data));
			}
			
			if constexpr (std::is_same_v<T, float>)
			{
				return _mm_castps_si128(_mm_set1_ps(data));
			}
			
			if constexpr (std::is_same_v<T, double>)
			{
				return _mm_castpd_si128(_mm_set1_pd(data));
			}
		}

		/// <summary>
		/// Converts the provided __m128i back into the original type
		/// </summary>
		/// <param name="data">__m128i to be transformed</param>
		/// <returns>The decoded value</returns>
		constexpr __forceinline T Decode(__m128i data)
		{
			if constexpr (std::is_pointer_v<T>)
			{
				return reinterpret_cast<T>(_mm_cvtsi128_si64(data));
			}
			
			if constexpr (std::is_integral_v<T>)
			{
				return static_cast<T>(_mm_cvtsi128_si64(data));
			}
			
			if constexpr (std::is_same_v<T, float>)
			{
				return _mm_cvtss_f32(_mm_castsi128_ps(data));
			}
			
			if constexpr (std::is_same_v<T, double>)
			{
				return _mm_cvtsd_f64(_mm_castsi128_pd(data));
			}
		}

		/// <summary>
		/// Computes the seed for a key using XorValue's 'Seed' template argument and the line number
		/// </summary>
		/// <returns>Pseudo-random seed for a key</returns>
		template <uint32_t LineNumber>
		static constexpr uint64_t ComputeKeySeed()
		{
			return Seed % (LineNumber + 1);
		}

		/// <summary>
		/// Computers the value for a key using the key seed and the UNIQUE_SEED64 macro
		/// </summary>
		/// <returns>Pseudo-random value for a key</returns>
		template <uint32_t LineNumber>
		static constexpr uint64_t ComputeKeyValue()
		{
			return UNIQUE_SEED64 + ComputeKeySeed<LineNumber>();
		}

		__m128i m_encryptedData = {};

		// The constant keys used for encryption/decryption
		XorConstant<uint64_t, ComputeKeyValue<USABLE_LINE>(), ComputeKeySeed<USABLE_LINE>()> Key1 = {};
		XorConstant<uint64_t, ComputeKeyValue<USABLE_LINE>(), ComputeKeySeed<USABLE_LINE>()> Key2 = {};
		XorConstant<uint64_t, ComputeKeyValue<USABLE_LINE>(), ComputeKeySeed<USABLE_LINE>()> Key3 = {};
	};

	/// <summary>
	/// A utility function to create an encrypted variable
	/// </summary>
	/// <typeparam name="T">The data type of the value</typeparam>
	/// <typeparam name="Seed">The seed used for encryption</typeparam>
	/// <param name="value">The value to be encrypted</param>
	/// <returns>An instance of XorValue</returns>
	template <typename T, uint64_t Seed>
	constexpr auto MakeXorVariableImpl(T value)
	{
		return XorValue<T, Seed>(value);
	}
}