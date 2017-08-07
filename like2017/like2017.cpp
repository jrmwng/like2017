// like2017.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "linq.h"
#include "simd_string.h"

#include <vector>
#include <iostream>

template <typename Tcontainer>
std::ostream & operator << (std::ostream & os, jrmwng::linq::linq_enumerable<Tcontainer> const & container)
{
	for (auto value : container)
	{
		os << value << std::ends;
	}
	return os;
}

int main(int nArg, char *apArg[])
{
	using simd_string = jrmwng::avx_string<char>;

	std::vector<simd_string> vectorArg(apArg + 1, apArg + nArg);

	std::cout << jrmwng::linq::from(vectorArg).order_by<std::less>() << std::endl;
	return 0;
}
