#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq
{
	TEST_CLASS(UnitTest_linq_concat)
	{
	public:

		TEST_METHOD(TestMethod_linq_concat)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> vectorName0({ "Jeremy", "Cally" });
			std::vector<Tsimd_string> vectorName1({ "Jeremy" });
			std::vector<Tsimd_string> vectorName2({ "Cally" });

			Assert::IsFalse(std::inner_product(
				vectorName0.begin(), vectorName0.end(),
				jrmwng::linq::from(vectorName1).concat(jrmwng::linq::from(vectorName2)).begin(),
				false,
				std::bit_or<bool>(),
				std::not_equal_to<Tsimd_string>()));

			Assert::IsTrue(std::inner_product(
				vectorName0.begin(), vectorName0.end(),
				jrmwng::linq::from(vectorName2).concat(jrmwng::linq::from(vectorName1)).begin(),
				false,
				std::bit_or<bool>(),
				std::not_equal_to<Tsimd_string>()));
		}

	};
}