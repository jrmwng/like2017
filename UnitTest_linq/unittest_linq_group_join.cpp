#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_group_join
{
	TEST_CLASS(UnitTest_linq_group_join)
	{
	public:

		TEST_METHOD(TestMethod_linq_group_join)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> const vectorName0({ "Jeremy", "Cally", "Bianca", "Summon" });

			auto const linq0 = jrmwng::linq::from(vectorName0)
				.group_join(vectorName0, std::mem_fun_ref(&Tsimd_string::length), std::mem_fun_ref(&Tsimd_string::length),
					[](auto const & objOuter, auto const & linqInner)
			{
				return linqInner.count();
			});
			std::vector<int> const vectorName1({ 3,1,3,3 });
			Assert::IsTrue(linq0.sequential_equal(vectorName1));

			auto const linq2 = jrmwng::linq::from(vectorName0)
				.group_join(vectorName0, std::mem_fun_ref(&Tsimd_string::length), std::mem_fun_ref(&Tsimd_string::length),
					[](auto const & objOuter, auto const & linqInner)
			{
				return objOuter;
			});
			Assert::IsTrue(linq2.sequential_equal(vectorName0));
		}

	};
}