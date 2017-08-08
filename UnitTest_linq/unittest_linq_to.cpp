#include "stdafx.h"
#include "CppUnitTest.h"

#include <deque>
#include <list>
#include <set>

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

		TEST_METHOD(TestMethod_linq_to_deque)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> const vectorName0({ "Jeremy", "Cally" });
			auto const dequeName1 = std::move(jrmwng::linq::from(vectorName0).to<std::deque>());

			Assert::IsTrue(jrmwng::linq::from(vectorName0).sequential_equal(dequeName1));
		}

		TEST_METHOD(TestMethod_linq_to_list)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> const vectorName0({ "Jeremy", "Cally" });
			auto const listName1 = std::move(jrmwng::linq::from(vectorName0).to<std::list>());

			Assert::IsTrue(jrmwng::linq::from(vectorName0).sequential_equal(listName1));
		}

		TEST_METHOD(TestMethod_linq_to_set)
		{
			using Tsimd_string = jrmwng::sse_string<char>;

			std::vector<Tsimd_string> const vectorName0({ "Jeremy", "Cally" });
			std::allocator<Tsimd_string> allocator1;
			auto const setName2 = std::move(jrmwng::linq::from(vectorName0).to<std::set>());
			auto const setName3 = std::move(jrmwng::linq::from(vectorName0).to<std::set>(allocator1));
			auto const setName4 = std::move(jrmwng::linq::from(vectorName0).to<std::set, std::greater<Tsimd_string>>());
			auto const setName5 = std::move(jrmwng::linq::from(vectorName0).to<std::set, std::greater<Tsimd_string>>(allocator1));
			auto const setName6 = std::move(jrmwng::linq::from(vectorName0).to<std::set<Tsimd_string>>());
			auto const setName7 = std::move(jrmwng::linq::from(vectorName0).to<std::set<Tsimd_string, std::greater<Tsimd_string>>>());

			Assert::IsFalse(jrmwng::linq::from(vectorName0).except(setName2).any());
			Assert::IsFalse(jrmwng::linq::from(vectorName0).except(setName3).any());
			Assert::IsFalse(jrmwng::linq::from(vectorName0).except(setName4).any());
			Assert::IsFalse(jrmwng::linq::from(vectorName0).except(setName5).any());
			Assert::IsFalse(jrmwng::linq::from(vectorName0).except(setName6).any());
			Assert::IsFalse(jrmwng::linq::from(vectorName0).except(setName7).any());
		}

	};
}