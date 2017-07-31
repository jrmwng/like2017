#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq
{
	TEST_CLASS(UnitTest_linq_count)
	{
	public:

		TEST_METHOD(TestMethod_linq_count)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> vectorName0({ "Jeremy", "Cally" });

			Assert::AreEqual<int>(2, jrmwng::linq::from(vectorName0).count());
		}

	};
}