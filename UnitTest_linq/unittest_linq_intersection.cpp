#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq
{
	TEST_CLASS(UnitTest_linq_intersection)
	{
	public:

		TEST_METHOD(TestMethod_linq_intersection)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> vectorName0({ "Jeremy", "Cally" });
			std::vector<Tsimd_string> vectorName1({ "Jeremy" });
			std::vector<Tsimd_string> vectorName2({ "Cally" });
			std::vector<Tsimd_string> vectorName3;

			Assert::IsTrue(jrmwng::linq::from(vectorName0).intersection(jrmwng::linq::from(vectorName1))
				.sequential_equal(vectorName1));
			Assert::IsTrue(jrmwng::linq::from(vectorName0).intersection(vectorName1)
				.sequential_equal(vectorName1));
			Assert::IsTrue(jrmwng::linq::from(vectorName0).intersection(vectorName2)
				.sequential_equal(vectorName2));
			Assert::IsFalse(jrmwng::linq::from(vectorName1).intersection(vectorName2).any());
			Assert::IsFalse(jrmwng::linq::from(vectorName0).intersection(vectorName3).any());
		}

	};
}