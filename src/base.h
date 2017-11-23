#pragma once

#include <list>
#include <queue>
#include <array>
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <functional>
#include <unordered_set>
#include <unordered_map>

#define ASSERT(cond)								assert(cond)

#define SAFE_DELETE(p)								if (p) { delete (p); (p) = nullptr; }
#define SAFE_RELEASE(p)								if (p) { (p)->Release(); (p) = nullptr; }

#define BIND_0(func, ...)			std::bind(&func, ##__VA_ARGS__)
#define BIND_1(func, ...)			std::bind(&func, ##__VA_ARGS__, std::placeholders::_1)
#define BIND_2(func, ...)			std::bind(&func, ##__VA_ARGS__, std::placeholders::_1, std::placeholders::_2)
#define BIND_3(func, ...)			std::bind(&func, ##__VA_ARGS__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

//	unicode ascii
#if defined(UNICODE)
#define stdstring		std::wstring
#else
#define stdstring		std::string
#endif