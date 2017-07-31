#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq
{
	TEST_CLASS(UnitTest_linq_order_by)
	{
	public:

		TEST_METHOD(TestMethod_linq_order_by)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> vectorName0({ "Jeremy", "Cally" });
			std::vector<Tsimd_string> vectorName1({ "Cally", "Jeremy" });
			auto const enumName2 = jrmwng::linq::from(vectorName0).order_by(std::identity<Tsimd_string>());
			std::vector<Tsimd_string> vectorName2(enumName2.begin(), enumName2.end());

			Assert::IsFalse(std::inner_product(vectorName1.begin(), vectorName1.end(), vectorName2.begin(), false, std::bit_or<bool>(), std::not_equal_to<Tsimd_string>()));
		}

	};
}