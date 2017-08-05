#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq
{
	TEST_CLASS(UnitTest_linq_except)
	{
	public:

		TEST_METHOD(TestMethod_linq_except)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> vectorName0({ "Jeremy", "Cally" });
			std::vector<Tsimd_string> vectorName1({ "Jeremy" });
			std::vector<Tsimd_string> vectorName2({ "Cally" });

			Assert::IsTrue(jrmwng::linq::from(vectorName0).except(jrmwng::linq::from(vectorName1))
				.sequential_equal(vectorName2));
			Assert::IsTrue(jrmwng::linq::from(vectorName0).except(vectorName1)
				.sequential_equal(vectorName2));

			Assert::IsFalse(jrmwng::linq::from(vectorName0).except(vectorName0).any());
			Assert::IsFalse(jrmwng::linq::from(vectorName1).except(vectorName1).any());
			Assert::IsFalse(jrmwng::linq::from(vectorName2).except(vectorName2).any());

			Assert::IsTrue(jrmwng::linq::from(vectorName1).except(vectorName2).sequential_equal(vectorName1));
			Assert::IsTrue(jrmwng::linq::from(vectorName2).except(vectorName1).sequential_equal(vectorName2));
		}

	};
}