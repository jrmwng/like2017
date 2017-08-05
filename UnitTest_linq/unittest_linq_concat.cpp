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

			Assert::IsTrue(jrmwng::linq::from(vectorName1).concat(jrmwng::linq::from(vectorName2))
				.sequential_equal(vectorName0));
			Assert::IsTrue(jrmwng::linq::from(vectorName1).concat(vectorName2)
				.sequential_equal(vectorName0));
			Assert::IsFalse(jrmwng::linq::from(vectorName2).concat(jrmwng::linq::from(vectorName1))
				.sequential_equal(vectorName0));
			Assert::IsFalse(jrmwng::linq::from(vectorName2).concat(vectorName1)
				.sequential_equal(vectorName0));
		}

	};
}