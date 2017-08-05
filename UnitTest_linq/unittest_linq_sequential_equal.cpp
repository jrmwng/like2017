#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq
{
	TEST_CLASS(UnitTest_linq_sequential_equal)
	{
	public:

		TEST_METHOD(TestMethod_linq_sequential_equal)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> vectorName0({ "Jeremy", "Cally" });
			std::vector<Tsimd_string> vectorName1({ "Jeremy", "Cally" });
			std::vector<Tsimd_string> vectorName2({ "Cally", "Jeremy" });
			std::vector<Tsimd_string> vectorName3({ "Jeremy" });

			Assert::IsTrue(jrmwng::linq::from(vectorName0).sequential_equal(jrmwng::linq::from(vectorName1)));
			Assert::IsFalse(jrmwng::linq::from(vectorName0).sequential_equal(jrmwng::linq::from(vectorName2)));
			Assert::IsFalse(jrmwng::linq::from(vectorName0).sequential_equal(jrmwng::linq::from(vectorName3)));
		}

	};
}