#include "stdafx.h"
#include "CppUnitTest.h"

#include "..\like2017\cnn.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_linq
{
	TEST_CLASS(UnitTest_cnn)
	{
	public:

		TEST_METHOD(TestMethod_cnn)
		{
			jrmwng::cnn::Ctensor<unsigned char, 1, 28, 28> aaaImageInput;
			{
				aaaImageInput = 1;
			}
			jrmwng::cnn::Ctensor<char, 36, 1, 5, 5> aaaaKernelConvolution;
			{
				aaaaKernelConvolution = 1;
			}
			jrmwng::cnn::Ctensor<short, 36, 1, 24, 24> aaaaImageInputConvoluted;
			{
				for (size_t uIndex = 0; uIndex < 36; uIndex++)
				{
					aaaaImageInputConvoluted[uIndex] = jrmwng::cnn::convolute<short>(aaaImageInput, aaaaKernelConvolution[uIndex]);
				}
			}
			short w0 = aaaaImageInputConvoluted[0][0][0][0];
			short w1 = aaaaImageInputConvoluted[0][0][23][23];
			Assert::IsTrue(25 == w0);
			Assert::IsTrue(25 == w1);

			jrmwng::cnn::Ctensor<short, 1, 36, 1, 12, 12> aaaaaImageInputConvolutedPooled;
			{
				aaaaaImageInputConvolutedPooled[0] = jrmwng::cnn::pooling<jrmwng::cnn::Cmax, 1, 1, 2, 2>(aaaaImageInputConvoluted);
			}
			short w2 = aaaaaImageInputConvolutedPooled[0][0][0][0][0];
			short w3 = aaaaaImageInputConvolutedPooled[0][0][0][11][11];
			Assert::IsTrue(25 == w2);
			Assert::IsTrue(25 == w3);

			jrmwng::cnn::Ctensor<short, 10, 36, 1, 12, 12> aaaaaFullyConnected;
			{
				aaaaaFullyConnected = 1;
			}
			jrmwng::cnn::Ctensor<int, 10, 1, 1, 1, 1> aaaaaScore;
			{
				aaaaaScore = jrmwng::cnn::convolute<int>(aaaaaFullyConnected, aaaaaImageInputConvolutedPooled);
			}
			int n0 = aaaaaScore[0][0][0][0][0];
			int n1 = aaaaaScore[1][0][0][0][0];
			Assert::IsTrue(25 * 12 * 12 * 36 == n0);
			Assert::IsTrue(25 * 12 * 12 * 36 == n1);
		}

	};
}