#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq
{
	TEST_CLASS(UnitTest_linq_to)
	{
	public:

		TEST_METHOD(TestMethod_linq_to_vector)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> const vectorName0({ "Jeremy", "Cally" });
			auto const vectorName1 = std::move(jrmwng::linq::from(vectorName0).to<std::vector>());

			Assert::IsTrue(jrmwng::linq::from(vectorName0).sequential_equal(vectorName1));
		}

	};
}