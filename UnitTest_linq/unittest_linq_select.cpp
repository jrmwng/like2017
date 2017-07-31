#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq
{
	TEST_CLASS(UnitTest_linq_select)
	{
	public:

		TEST_METHOD(TestMethod_linq_select)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> vectorName0({ "Jeremy", "Cally" });
			std::vector<char> vectorName1({ 'J','C' });
			auto const enumName2 = jrmwng::linq::from(vectorName0).select(
				[](auto const & str)
			{
				return str.c_str()[0];
			});
			std::vector<char> vectorName2(enumName2.begin(), enumName2.end());

			Assert::IsFalse(std::inner_product(vectorName1.begin(), vectorName1.end(), vectorName2.begin(), false, std::bit_or<bool>(), std::not_equal_to<char>()));
		}

	};
}