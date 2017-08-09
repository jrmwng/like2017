#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq
{
	TEST_CLASS(UnitTest_linq_select_many)
	{
	public:

		TEST_METHOD(TestMethod_linq_select_many)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> vectorName0({ "Jeremy", "Cally" });
			std::vector<Tsimd_string> vectorName1({ "Cally", "Jeremy" });


			auto vectorName2 = jrmwng::linq::from(vectorName0)
				.select_many([&](auto str0)
			{
				return jrmwng::linq::from(vectorName0);
			})
				.order_by<std::less>()
				.to<std::vector>();


			Assert::IsTrue(jrmwng::linq::from(vectorName0)
				.select_many([&](auto str0)
			{
				return jrmwng::linq::from(vectorName0);
			})
				.sequential_equal(jrmwng::linq::from(vectorName0).concat(vectorName0)));


			Assert::IsTrue(jrmwng::linq::from(vectorName0)
				.select_many([&](auto str0)
			{
				return jrmwng::linq::from(vectorName0)
					.where([nLength = str0.length()](auto str1)
				{
					return str1.length() != nLength;
				});
			})
				.sequential_equal(jrmwng::linq::from(vectorName1)));
		}

	};
}