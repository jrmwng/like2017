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

			Assert::IsTrue(jrmwng::linq::from(vectorName0).order_by(std::identity<Tsimd_string>())
				.sequential_equal(vectorName1));
			Assert::IsFalse(jrmwng::linq::from(vectorName1).order_by(std::identity<Tsimd_string>())
				.sequential_equal(vectorName0));

			Assert::IsTrue(jrmwng::linq::from(vectorName1).order_by(std::identity<Tsimd_string>(), std::greater<Tsimd_string>())
				.sequential_equal(vectorName0));
			Assert::IsFalse(jrmwng::linq::from(vectorName0).order_by(std::identity<Tsimd_string>(), std::greater<Tsimd_string>())
				.sequential_equal(vectorName1));
		}

	};
}