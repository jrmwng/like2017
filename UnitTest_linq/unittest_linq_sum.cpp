#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq
{
	TEST_CLASS(UnitTest_linq_sum)
	{
	public:

		TEST_METHOD(TestMethod_linq_sum)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> vectorName0({ "Jeremy", "Cally" });
			std::vector<int> vectorName1({ 1,2,3 });

			Assert::AreEqual(11, jrmwng::linq::from(vectorName0).sum<int>(std::mem_fun_ref(&Tsimd_string::length)));
			Assert::AreEqual(6, jrmwng::linq::from(vectorName1).sum<int>());
		}

	};
}