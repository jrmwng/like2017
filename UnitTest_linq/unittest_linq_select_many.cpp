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

			Assert::IsTrue(jrmwng::linq::from(vectorName0)
				.select_many([&](auto str0)
			{
				return jrmwng::linq::from(vectorName0);
			})
				.sequential_equal(jrmwng::linq::from(vectorName0).concat(vectorName0)));
		}

	};
}