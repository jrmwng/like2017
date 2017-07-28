#pragma once

#include <intrin.h>

namespace jrmwng
{
	namespace detail
	{
		template <typename Tmm, typename Tchar>
		class simd_string;

		template <>
		class simd_string<__m128i, char>
		{
		protected:
			__m128i m_xmm;

			simd_string(simd_string const & that)
				: m_xmm(that.m_xmm)
			{}
			simd_string(char const *pc)
			{
				std::char_traits<char>::_Copy_s(m_xmm.m128i_i8, 16, pc, 15);
			}

			int cmplt(simd_string const & that) const
			{
				__m128i const b16This = _mm_add_epi8(_mm_set1_epi8(-0x80), this->m_xmm);
				__m128i const b16That = _mm_add_epi8(_mm_set1_epi8(-0x80), that.m_xmm);
				__m128i const b16CmpLt = _mm_cmplt_epi8(b16This, b16That);
				int const nCmpLt = _mm_movemask_epi8(b16CmpLt);
				int const nTzCnt = _tzcnt_u32(nCmpLt);
				return nTzCnt;
			}
			int cmpeq(simd_string const & that) const
			{
				__m128i const b16That = that.m_xmm;
				__m128i const b16This = m_xmm;
				return _mm_cmpistrc(b16That, b16This, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);
			}
		public:
			char const * c_str() const
			{
				return m_xmm.m128i_i8;
			}

			int find(char cPattern) const
			{
				__m128i const b1Pattern = _mm_cvtsi32_si128(cPattern);
				__m128i const b16This = m_xmm;
				int const nIndex = _mm_cmpistri(b1Pattern, b16This, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ORDERED | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);
				return nIndex;
			}
			int find(simd_string const & that) const
			{
				__m128i const b16That = that.m_xmm;
				__m128i const b16This = m_xmm;
				int const nIndex = _mm_cmpistri(b16That, b16This, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ORDERED | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);
				return nIndex;
			}
			int rfind(char cPattern) const
			{
				__m128i const b1Pattern = _mm_cvtsi32_si128(cPattern);
				__m128i const b16This = m_xmm;
				int const nIndex = _mm_cmpistri(b1Pattern, b16This, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ORDERED | _SIDD_POSITIVE_POLARITY | _SIDD_MOST_SIGNIFICANT);
				return nIndex;
			}
			int rfind(simd_string const & that) const
			{
				__m128i const b16That = that.m_xmm;
				__m128i const b16This = m_xmm;
				int const nIndex = _mm_cmpistri(b16That, b16This, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ORDERED | _SIDD_POSITIVE_POLARITY | _SIDD_MOST_SIGNIFICANT);
				return nIndex;
			}
			int find_first_of(char cPattern) const
			{
				__m128i const b1Pattern = _mm_cvtsi32_si128(cPattern);
				__m128i const b16This = m_xmm;
				int const nIndex = _mm_cmpistri(b1Pattern, b16This, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);
				return nIndex;
			}
			int find_first_of(simd_string const & that) const
			{
				__m128i const b1Pattern = that.m_xmm;
				__m128i const b16This = m_xmm;
				int const nIndex = _mm_cmpistri(b1Pattern, b16This, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);
				return nIndex;
			}
			int find_first_not_of(char cPattern) const
			{
				__m128i const b1Pattern = _mm_cvtsi32_si128(cPattern);
				__m128i const b16This = m_xmm;
				int const nIndex = _mm_cmpistri(b1Pattern, b16This, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);
				return nIndex;
			}
			int find_first_not_of(simd_string const & that) const
			{
				__m128i const b1Pattern = that.m_xmm;
				__m128i const b16This = m_xmm;
				int const nIndex = _mm_cmpistri(b1Pattern, b16This, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);
				return nIndex;
			}
			int find_last_of(char cPattern) const
			{
				__m128i const b1Pattern = _mm_cvtsi32_si128(cPattern);
				__m128i const b16This = m_xmm;
				int const nIndex = _mm_cmpistri(b1Pattern, b16This, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_POSITIVE_POLARITY | _SIDD_MOST_SIGNIFICANT);
				return nIndex;
			}
			int find_last_of(simd_string const & that) const
			{
				__m128i const b1Pattern = that.m_xmm;
				__m128i const b16This = m_xmm;
				int const nIndex = _mm_cmpistri(b1Pattern, b16This, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_POSITIVE_POLARITY | _SIDD_MOST_SIGNIFICANT);
				return nIndex;
			}
			int find_last_not_of(char cPattern) const
			{
				__m128i const b1Pattern = _mm_cvtsi32_si128(cPattern);
				__m128i const b16This = m_xmm;
				int const nIndex = _mm_cmpistri(b1Pattern, b16This, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_NEGATIVE_POLARITY | _SIDD_MOST_SIGNIFICANT);
				return nIndex;
			}
			int find_last_not_of(simd_string const & that) const
			{
				__m128i const b1Pattern = that.m_xmm;
				__m128i const b16This = m_xmm;
				int const nIndex = _mm_cmpistri(b1Pattern, b16This, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_NEGATIVE_POLARITY | _SIDD_MOST_SIGNIFICANT);
				return nIndex;
			}
		};
		template <>
		class simd_string<__m128i, char16_t>
		{
		protected:
			__m128i m_xmm;

			simd_string(simd_string const & that)
				: m_xmm(that.m_xmm)
			{}
			simd_string(char16_t const *pc)
				: m_xmm(_mm_setzero_si128())
			{
				std::char_traits<char16_t>::_Copy_s(reinterpret_cast<char16_t*>(m_xmm.m128i_i16), 8, pc, 7);
			}

			int cmplt(simd_string const & that) const
			{
				__m128i const w8This = _mm_add_epi16(_mm_set1_epi16(-0x8000), this->m_xmm);
				__m128i const w8That = _mm_add_epi16(_mm_set1_epi16(-0x8000), that.m_xmm);
				__m128i const w8CmpLt = _mm_cmplt_epi16(w8This, w8That);
				int const nCmpLt = _mm_movemask_epi8(w8CmpLt);
				int const nTzCnt = _tzcnt_u32(nCmpLt);
				return nTzCnt;
			}
			int cmpeq(simd_string const & that) const
			{
				__m128i const b16That = that.m_xmm;
				__m128i const b16This = m_xmm;
				return _mm_cmpistrc(b16That, b16This, _SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);
			}
		public:
			char16_t const * c_str() const
			{
				return reinterpret_cast<char16_t const*>(m_xmm.m128i_i16);
			}

			int find(char16_t cPattern) const
			{
				__m128i const uw1Pattern = _mm_cvtsi32_si128(cPattern);
				__m128i const uw8This = m_xmm;
				int const nIndex = _mm_cmpistri(uw1Pattern, uw8This, _SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_ORDERED | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);
				return nIndex;
			}
			int find(simd_string const & that) const
			{
				__m128i const b16That = that.m_xmm;
				__m128i const b16This = m_xmm;
				int const nIndex = _mm_cmpistri(b16That, b16This, _SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_ORDERED | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);
				return nIndex;
			}
		};
		template <>
		class simd_string<__m128i, char32_t>
		{
		protected:
			__m128i m_xmm;

			simd_string(simd_string const & that)
				: m_xmm(that.m_xmm)
			{}
			simd_string(char32_t const *pc)
				: m_xmm(_mm_setzero_si128())
			{
				std::char_traits<char32_t>::_Copy_s(reinterpret_cast<char32_t*>(m_xmm.m128i_i32), 4, pc, 3);
			}

			int cmplt(simd_string const & that) const
			{
				__m128i const l4This = _mm_add_epi32(_mm_set1_epi32(-0x80000000LL), this->m_xmm);
				__m128i const l4That = _mm_add_epi32(_mm_set1_epi32(-0x80000000LL), that.m_xmm);
				__m128i const l4CmpLt = _mm_cmplt_epi32(l4This, l4That);
				int const nCmpLt = _mm_movemask_epi8(l4CmpLt);
				int const nTzCnt = _tzcnt_u32(nCmpLt);
				return nTzCnt;
			}
			bool cmpeq(simd_string const & that) const
			{
				__m128i const l4This = this->m_xmm;
				__m128i const l4That = that.m_xmm;
				__m128i const l4CmpEq = _mm_cmpeq_epi32(l4This, l4That);
				int const nCmpEq = _mm_movemask_epi8(l4CmpEq);
				return nCmpEq == 0xFFFF;
			}
		public:
			char32_t const * c_str() const
			{
				return reinterpret_cast<char32_t const*>(m_xmm.m128i_i32);
			}

			int find(char32_t cPattern) const
			{
				__m128i const l4Pattern = _mm_set1_epi32(cPattern);
				__m128i const l4This = m_xmm;
				__m128i const l4CmpEq = _mm_cmpeq_epi32(l4Pattern, l4This);
				int const nCmpEq = _mm_movemask_ps(_mm_castsi128_ps(l4CmpEq));
				int const nIndex = _tzcnt_u32(nCmpEq);
				return nIndex;
			}
		};
		template <>
		class simd_string<__m256i, char>
		{
		protected:
			__m256i m_ymm;

			simd_string(simd_string const & that)
				: m_ymm(that.m_ymm)
			{}
			simd_string(char const *pc)
				: m_ymm(_mm256_setzero_si256())
			{
				std::char_traits<char>::_Copy_s(m_ymm.m256i_i8, 32, pc, 31);
			}

			int cmplt(simd_string const & that) const
			{
				__m256i const b32This = _mm256_add_epi8(_mm256_set1_epi8(-0x80), this->m_ymm);
				__m256i const b32That = _mm256_add_epi8(_mm256_set1_epi8(-0x80), that.m_ymm);
				__m256i const b32CmpLt = _mm256_cmpgt_epi8(b32That, b32This);
				int const nCmpLt = _mm256_movemask_epi8(b32CmpLt);
				int const nTzCnt = _tzcnt_u32(nCmpLt);
				return nTzCnt;
			}
			bool cmpeq(simd_string const & that) const
			{
				__m256i const b32This = this->m_ymm;
				__m256i const b32That = that.m_ymm;
				__m256i const b32CmpEq = _mm256_cmpeq_epi8(b32This, b32That);
				int const nCmpEq = _mm256_movemask_epi8(b32CmpEq);
				return ~nCmpEq == 0;
			}
		public:
			char const * c_str() const
			{
				return m_ymm.m256i_i8;
			}

			int find(char cPattern) const
			{
				__m256i const b32This = m_ymm;
				__m256i const b32Pattern = _mm256_set1_epi8(cPattern);
				__m256i const b32CmpEq = _mm256_cmpeq_epi8(b32This, b32Pattern);
				int const nMask = _mm256_movemask_epi8(b32CmpEq);
				int const nIndex = _tzcnt_u32(nMask);
				return nIndex;
			}
		};
	}
	template <typename Tmm, typename Tchar>
	class simd_string
		: public detail::simd_string<Tmm, Tchar>
	{
		using base_t = detail::simd_string<Tmm, Tchar>;
	public:
		simd_string(Tchar const *pc)
			: base_t(pc)
		{}
		int compare(simd_string const & that) const
		{
			//       [1 2 3 ...]
			//       [1 3 2 ...]
			// cmplt [0 1 0 ...]
			// cmpgt [0 0 1 ...]
			return cmplt(that) - that.cmplt(*this);
		}
		bool operator < (simd_string const & that) const
		{
			return cmplt(that) < that.cmplt(*this);
		}
		bool operator <= (simd_string const & that) const
		{
			return cmplt(that) <= that.cmplt(*this);
		}
		bool operator == (simd_string const & that) const
		{
			return cmplt(that) == that.cmplt(*this);
		}
		bool operator != (simd_string const & that) const
		{
			return cmplt(that) != that.cmplt(*this);
		}
		bool operator >= (simd_string const & that) const
		{
			return cmplt(that) >= that.cmplt(*this);
		}
		bool operator > (simd_string const & that) const
		{
			return cmplt(that) > that.cmplt(*this);
		}

		int length() const
		{
			return base_t::find(Tchar(0));
		}
	};
	template <typename Tmm>
	std::ostream & operator << (std::ostream & os, simd_string<Tmm, char> const & str)
	{
		return os << str.c_str();
	}

	template <typename Tchar>
	using sse_string = simd_string<__m128i, Tchar>;
	template <typename Tchar>
	using avx_string = simd_string<__m256i, Tchar>;
}