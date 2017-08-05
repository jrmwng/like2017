#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq
{
	TEST_CLASS(UnitTest_linq_select)
	{
	public:

		TEST_METHOD(TestMethod_linq_select)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> vectorName0({ "Jeremy", "Cally" });
			std::vector<char> vectorName1({ 'J','C' });

			Assert::IsTrue(jrmwng::linq::from(vectorName0).select(
				[](auto const & str)
			{
				return str.c_str()[0];
			}).sequential_equal(vectorName1));

			Assert::IsFalse(jrmwng::linq::from(vectorName0).select(
				[](auto const & str)
			{
				return str.c_str()[1];
			}).sequential_equal(vectorName1));
		}

	};
}