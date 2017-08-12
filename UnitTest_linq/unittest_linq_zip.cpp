#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq
{
	TEST_CLASS(UnitTest_linq_zip)
	{
	public:

		TEST_METHOD(TestMethod_linq_zip)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> vectorName0({ "Jeremy", "Cally" });
			std::vector<Tsimd_string> vectorName1({ "Cally", "Jeremy" });

			auto vectorName2 = jrmwng::linq::from(vectorName0)
				.zip(vectorName1, [](auto str0, auto str1)
			{
				return str0.length() + str1.length();
			})
				.sum<int>();

			Assert::IsTrue(jrmwng::linq::from(vectorName0)
				.zip(vectorName1, [](auto str0, auto str1)
			{
				return str0;
			})
				.sequential_equal(vectorName0));

			Assert::IsTrue(jrmwng::linq::from(vectorName0)
				.zip(vectorName1, [](auto str0, auto str1)
			{
				return str1;
			})
				.sequential_equal(vectorName1));
		}

	};
}