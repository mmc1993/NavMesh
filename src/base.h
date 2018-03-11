#pragma once

#include "third/sformat.h"
#include <list>
#include <queue>
#include <array>
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <variant>
#include <utility>
#include <optional>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <unordered_set>
#include <unordered_map>

#define ASSERT(cond)								assert(cond)

#define SAFE_DELETE(p)								if (p) { delete (p); (p) = nullptr; }

template <class T>
inline unsigned long SafeRelease(T & p)
{
	unsigned long ret = ULONG_MAX;
	ret = p->Release();
	p = nullptr;
	return ret;
}

#define Log(fmt, ...)	std::cout << SFormat(fmt, ##__VA_ARGS__) << std::endl;

#define BIND_0(func, ...)			std::bind(&func, ##__VA_ARGS__)
#define BIND_1(func, ...)			std::bind(&func, ##__VA_ARGS__, std::placeholders::_1)
#define BIND_2(func, ...)			std::bind(&func, ##__VA_ARGS__, std::placeholders::_1, std::placeholders::_2)
#define BIND_3(func, ...)			std::bind(&func, ##__VA_ARGS__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
