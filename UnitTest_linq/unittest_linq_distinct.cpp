#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_distinct
{
	TEST_CLASS(UnitTest_distinct)
	{
	public:

		TEST_METHOD(TestMethod_distinct)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> const vectorName0({ "Jeremy", "Cally", "Bianca", "Summon", "Cally" });
			std::vector<Tsimd_string> const vectorName1({ "Jeremy", "Cally", "Bianca", "Summon" });
			std::vector<Tsimd_string> const vectorName2({ "Jeremy", "Cally" });

			Assert::IsTrue(jrmwng::linq::from(vectorName0)
				.distinct()
				.sequential_equal(vectorName1));

			Assert::IsTrue(jrmwng::linq::from(vectorName0)
				.distinct([](auto const &left, auto const &right)
			{
				return left.length() == right.length();
			})
				.sequential_equal(vectorName2));
		}

	};
}