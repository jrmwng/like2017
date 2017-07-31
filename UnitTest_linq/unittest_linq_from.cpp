#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq
{		
	TEST_CLASS(UnitTest_linq_from)
	{
	public:
		
		TEST_METHOD(TestMethod_linq_from)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> vectorName0({ "Jeremy", "Cally" });
			std::vector<Tsimd_string> vectorName1;

			for (auto str : jrmwng::linq::from(vectorName0))
			{
				vectorName1.emplace_back(str);
			}

			Assert::IsFalse(std::inner_product(vectorName0.begin(), vectorName0.end(), vectorName1.begin(), false, std::bit_or<bool>(), std::not_equal_to<Tsimd_string>()));
		}

	};
}