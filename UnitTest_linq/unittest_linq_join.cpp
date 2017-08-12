#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq_join
{
	TEST_CLASS(UnitTest_linq_join)
	{
	public:

		TEST_METHOD(TestMethod_linq_join)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> const vectorName0({ "Jeremy", "Cally", "Bianca", "Summon" });
			std::vector<Tsimd_string> const vectorName1({ "Jeremy", "Bianca", "Summon", "Cally", "Jeremy", "Bianca", "Summon", "Jeremy", "Bianca", "Summon" });
			std::vector<Tsimd_string> const vectorName2({ "Jeremy", "Jeremy", "Jeremy", "Cally", "Bianca", "Bianca", "Bianca", "Summon", "Summon", "Summon" });

			Assert::IsTrue(jrmwng::linq::from(vectorName0)
				.join(vectorName0, std::mem_fun_ref(&Tsimd_string::length), std::mem_fun_ref(&Tsimd_string::length),
					[](auto const & strOuter, auto const & strInner)
			{
				return strInner;
			})
				.sequential_equal(vectorName1));

			Assert::IsTrue(jrmwng::linq::from(vectorName0)
				.join(vectorName0, std::mem_fun_ref(&Tsimd_string::length), std::mem_fun_ref(&Tsimd_string::length),
					[](auto const & strOuter, auto const & strInner)
			{
				return strOuter;
			})
				.sequential_equal(vectorName2));
		}

	};
}