#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_group_by
{
	TEST_CLASS(UnitTest_linq_group_by)
	{
	public:

		TEST_METHOD(TestMethod_linq_group_by)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> const vectorName0({ "Jeremy", "Cally", "Bianca", "Summon" });
			std::vector<ptrdiff_t> const vectorGroup1({ 3, 1 });

			Assert::IsTrue(jrmwng::linq::from(vectorName0)
				.group_by([](auto str)
			{
				return str.length();
			})
				.select([](auto groupByLength)
			{
				return groupByLength.count();
			})
				.sequential_equal(vectorGroup1));

			Assert::IsTrue(jrmwng::linq::from(vectorName0)
				.group_by(std::identity<Tsimd_string>(), [](auto left, auto right)
			{
				return left.length() == right.length();
			})
				.select([](auto groupByLength)
			{
				return groupByLength.count();
			})
				.sequential_equal(vectorGroup1));
		}

	};
}