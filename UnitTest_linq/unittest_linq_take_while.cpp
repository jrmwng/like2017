#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq
{
	TEST_CLASS(UnitTest_linq_take_while)
	{
	public:

		TEST_METHOD(TestMethod_linq_take_while)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> vectorName0({ "Jeremy", "Cally" });
			std::vector<Tsimd_string> vectorName1({ "Jeremy" });
			std::vector<Tsimd_string> vectorName2;

			Assert::IsTrue(jrmwng::linq::from(vectorName0).take_while([](auto const & str)
			{
				return str.c_str()[0] == 'J';
			}).sequential_equal(vectorName1));
			Assert::IsTrue(jrmwng::linq::from(vectorName0).take_while([](auto const &)
			{
				return true;
			}).sequential_equal(vectorName0));
			Assert::IsFalse(jrmwng::linq::from(vectorName0).take_while([](auto const &)
			{
				return false;
			}).any());
		}

	};
}