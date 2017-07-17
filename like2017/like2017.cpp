// like2017.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "linq.h"

#include <vector>
#include <iostream>
#include <functional>


int main(int nArg, char *apArg[])
{
	std::vector<int> const vectorData({ 3, 0, 1, 1, 2, 2, 3 });

	for (int nData : vectorData)
	{
		std::cout << nData << std::ends;
	}
	std::cout << std::endl;

	for (int nData : jrmwng::linq::from(vectorData))
	{
		std::cout << nData << std::ends;
	}
	std::cout << std::endl;

	for (int nData : jrmwng::linq::from(vectorData)
		.where([](int n) {return n & 1; })
		.where([](int n) {return n & 2; }))
	{
		std::cout << nData << std::ends;
	}
	std::cout << std::endl;

	for (int nData : jrmwng::linq::from(vectorData).order_by(std::identity<int>()))
	{
		std::cout << nData << std::ends;
	}
	std::cout << std::endl;

	for (int nData : jrmwng::linq::from(vectorData)
		.order_by(std::identity<int>())
		.order_by(std::identity<int>(), std::greater<int>()))
	{
		std::cout << nData << std::ends;
	}
	std::cout << std::endl;

	for (int nData : jrmwng::linq::from(vectorData)
		.where([](int n) {return n & 1; })
		.order_by(std::identity<int>(), std::greater<int>()))
	{
		std::cout << nData << std::ends;
	}
	std::cout << std::endl;

	for (auto rangeData : jrmwng::linq::from(vectorData).group_by(std::identity<int>()))
	{
		for (int nData : rangeData)
		{
			std::cout << nData << std::ends;
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	for (auto rangeData : jrmwng::linq::from(vectorData)
		.order_by(std::identity<int>())
		.group_by(std::identity<int>())
		)
	{
		for (int nData : rangeData)
		{
			std::cout << nData << std::ends;
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	for (auto nData : jrmwng::linq::from(vectorData)
		.order_by(std::identity<int>())
		.group_by(std::identity<int>())
		.max(std::identity<int>(), std::less<int>())
		)
	{
		std::cout << nData << std::ends;
	}
	std::cout << std::endl;

	for (auto nData : jrmwng::linq::from(vectorData)
		.order_by(std::identity<int>())
		.group_by(std::identity<int>())
		.count()
		)
	{
		std::cout << nData << std::ends;
	}
	std::cout << std::endl;

	for (auto nData : jrmwng::linq::from(vectorData)
		.order_by(std::identity<int>())
		.group_by(std::identity<int>())
		.sum(std::identity<int>())
		)
	{
		std::cout << nData << std::ends;
	}
	std::cout << std::endl;

	std::cout << jrmwng::linq::from(vectorData)
		.order_by(std::identity<int>())
		.group_by(std::identity<int>())
		.sum(std::identity<int>())
		.sum(std::identity<int>())
		<< std::endl;

	return 0;
}
