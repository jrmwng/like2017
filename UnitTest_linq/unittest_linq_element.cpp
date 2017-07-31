#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq
{
	TEST_CLASS(UnitTest_linq_element)
	{
	public:

		TEST_METHOD(TestMethod_linq_min)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> const vectorName0({ "Jeremy", "Cally" });

			Assert::AreEqual<Tsimd_string>(Tsimd_string("Cally"), jrmwng::linq::from(vectorName0).min());
		}

		TEST_METHOD(TestMethod_linq_max)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> const vectorName0({ "Jeremy", "Cally" });

			Assert::AreEqual<Tsimd_string>(Tsimd_string("Jeremy"), jrmwng::linq::from(vectorName0).max());
		}

		TEST_METHOD(TestMethod_linq_first)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> const vectorName0({ "Jeremy", "Cally" });

			Assert::AreEqual<Tsimd_string>(Tsimd_string("Jeremy"), jrmwng::linq::from(vectorName0).first());
		}

		TEST_METHOD(TestMethod_linq_last)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> const vectorName0({ "Jeremy", "Cally" });

			Assert::AreEqual<Tsimd_string>(Tsimd_string("Cally"), jrmwng::linq::from(vectorName0).last());
		}

		TEST_METHOD(TestMethod_linq_element_at)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> const vectorName0({ "Jeremy", "Cally" });

			Assert::AreEqual<Tsimd_string>(Tsimd_string("Jeremy"), jrmwng::linq::from(vectorName0).element_at(0));
			Assert::AreEqual<Tsimd_string>(Tsimd_string("Cally"), jrmwng::linq::from(vectorName0).element_at(1));
		}

	};
}