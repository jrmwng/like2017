// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// Headers for CppUnitTest
#include "CppUnitTest.h"

// TODO: reference additional headers your program requires here
#include "..\like2017\linq.h"
#include "..\like2017\simd_string.h"
#include <vector>
#include <functional>

std::wstring ToString(const jrmwng::sse_string<char>& q);
