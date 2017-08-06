#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq
{
	TEST_CLASS(UnitTest_linq_where)
	{
	public:

		TEST_METHOD(TestMethod_linq_where)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> vectorName0({ "Jeremy", "Cally" });
			std::vector<Tsimd_string> vectorName1({ "Jeremy" });
			std::vector<Tsimd_string> vectorName2({ "Cally" });

			Assert::IsTrue(jrmwng::linq::from(vectorName0).where([](auto str)
			{
				return str.c_str()[0] == 'J';
			}).sequential_equal(vectorName1));
			Assert::IsTrue(jrmwng::linq::from(vectorName0).where([](auto str)
			{
				return str.c_str()[0] == 'C';
			}).sequential_equal(vectorName2));
			Assert::IsTrue(jrmwng::linq::from(vectorName0).where([](auto)
			{
				return true;
			}).sequential_equal(vectorName0));
			Assert::IsFalse(jrmwng::linq::from(vectorName0).where([](auto)
			{
				return false;
			}).any());
		}

	};
}