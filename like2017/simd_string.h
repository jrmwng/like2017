#pragma once

#include <intrin.h>
#include <string>

namespace jrmwng
{
	namespace detail
	{
		template <typename Tmm, typename Tchar>
		struct simd_string;

		template <>
		struct simd_string<__m128i, char>
		{
			__m128i m_xmm;

			simd_string(char const *pc)
			{
				strncpy_s(m_xmm.m128i_i8, 16, pc, 15);
			}

			int cmplt(simd_string const & that) const
			{
				__m128i const b16This = _mm_add_epi8(_mm_set1_epi8(-128), this->m_xmm);
				__m128i const b16That = _mm_add_epi8(_mm_set1_epi8(-128), that.m_xmm);
				__m128i const b16CmpLt = _mm_cmplt_epi8(b16This, b16That);
				int const nCmpLt = _mm_movemask_epi8(b16CmpLt);
				int const nTzCnt = _tzcnt_u32(nCmpLt);
				return nTzCnt;
			}

			operator char const * () const
			{
				return m_xmm.m128i_i8;
			}

			int index_of(char cPattern) const
			{
				__m128i const b1Pattern = _mm_cvtsi32_si128(cPattern);
				__m128i const b16This = m_xmm;
				int const nIndex = _mm_cmpestri(b1Pattern, 1, b16This, 16, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);
				return nIndex;
			}
		};
		template <>
		struct simd_string<__m128i, wchar_t>
		{
			__m128i m_xmm;

			simd_string(wchar_t const *pc)
				: m_xmm(_mm_setzero_si128())
			{
				wcsncpy_s(reinterpret_cast<wchar_t*>(m_xmm.m128i_u16), 8, pc, 7);
			}

			int cmplt(simd_string const & that) const
			{
				__m128i const w8This = _mm_add_epi16(_mm_set1_epi16(-32768), this->m_xmm);
				__m128i const w8That = _mm_add_epi16(_mm_set1_epi16(-32768), that.m_xmm);
				__m128i const w8CmpLt = _mm_cmplt_epi16(w8This, w8That);
				int const nCmpLt = _mm_movemask_epi8(w8CmpLt);
				int const nTzCnt = _tzcnt_u32(nCmpLt);
				return nTzCnt;
			}

			operator wchar_t const * () const
			{
				return reinterpret_cast<wchar_t const*>(m_xmm.m128i_i16);
			}

			int index_of(wchar_t cPattern) const
			{
				__m128i const uw1Pattern = _mm_cvtsi32_si128(cPattern);
				__m128i const uw8This = m_xmm;
				int const nIndex = _mm_cmpestri(uw1Pattern, 1, uw8This, 8, _SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_POSITIVE_POLARITY | _SIDD_LEAST_SIGNIFICANT);
				return nIndex;
			}
		};
		template <>
		struct simd_string<__m256i, char>
		{
			__m256i m_ymm;

			simd_string(char const *pc)
				: m_ymm(_mm256_setzero_si256())
			{
				strncpy_s(m_ymm.m256i_i8, pc, 15);
			}

			int cmplt(simd_string const & that) const
			{
				__m256i const b32This = _mm256_add_epi8(_mm256_set1_epi8(-128), this->m_ymm);
				__m256i const b32That = _mm256_add_epi8(_mm256_set1_epi8(-128), that.m_ymm);
				__m256i const b32CmpLt = _mm256_cmpgt_epi8(b32That, b32This);
				int const nCmpLt = _mm256_movemask_epi8(b32CmpLt);
				int const nTzCnt = _tzcnt_u32(nCmpLt);
				return nTzCnt;
			}

			operator char const * () const
			{
				return m_ymm.m256i_i8;
			}

			int index_of(char cPattern) const
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
		: detail::simd_string<Tmm, Tchar>
	{
		using base_t = detail::simd_string<Tmm, Tchar>;
	public:
		simd_string(Tchar const *pc)
			: base_t(pc)
		{}
		bool operator < (simd_string const & that) const
		{
			//       [1 2 3 ...]
			//       [1 3 2 ...]
			// cmplt [0 1 0 ...]
			// cmpgt [0 0 1 ...]
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

		operator Tchar const * () const
		{
			return base_t::operator Tchar const *();
		}

		int index_of(Tchar cPattern) const
		{
			return base_t::index_of(cPattern);
		}
		int length() const
		{
			return base_t::index_of(Tchar(0));
		}
	};
	template <typename Tmm>
	std::ostream & operator << (std::ostream & os, simd_string<Tmm, char> const & str)
	{
		return os << static_cast<char const*>(str);
	}
}