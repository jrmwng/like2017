#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq
{
	TEST_CLASS(UnitTest_linq_any)
	{
	public:

		TEST_METHOD(TestMethod_linq_any)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> vectorName0({ "Jeremy", "Cally" });
			std::vector<Tsimd_string> vectorName1;

			Assert::IsTrue(jrmwng::linq::from(vectorName0).any());
			Assert::IsFalse(jrmwng::linq::from(vectorName1).any());
			Assert::IsTrue(jrmwng::linq::from(vectorName0).any([](auto const & str) {return str.find('a') == 1; }));
			Assert::IsFalse(jrmwng::linq::from(vectorName0).any([](auto const & str) {return str.find('b') == 1; }));
		}

	};
}