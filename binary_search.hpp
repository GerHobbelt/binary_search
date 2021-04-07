/*
	Copyright (C) 2014-2021 Igor van den Hoven ivdhoven@gmail.com
	Copyright (C) 2021 Marcel Pi Nacy

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:
	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef BINARY_SEARCH_CPP
#define BINARY_SEARCH_CPP
#include <type_traits>
#include <iterator>
#include <cassert>



template <typename Iterator, typename Equal>
constexpr Iterator linear_search_base(Iterator begin, Iterator end, Equal&& equal_to)
{
	if (begin == end)
		return end;
	assert(begin < end);

	for (Iterator i = std::prev(end); i >= begin; --i)
		if (equal_to(*i))
			return i;

	return end;
}

template <typename Iterator, typename T, typename Equal>
constexpr Iterator linear_search(Iterator begin, Iterator end, T&& key, Equal&& equal_to)
{
	return ::linear_search_base(begin, end,
		[&](auto& right) { return equal_to(key, right); });
}

template <typename Iterator, typename T>
constexpr Iterator linear_search(Iterator begin, Iterator end, T&& key)
{
	return ::linear_search_base(begin, end,
		[&](auto& right) { return right == key; });
}

template <typename Collection, typename T>
constexpr auto linear_search(Collection&& collection, T&& key)
{
	return ::linear_search(collection.begin(), collection.end(), std::forward<T>(key));
}



template <typename Iterator, typename LessThan, typename Equal>
constexpr Iterator breaking_linear_search_base(Iterator begin, Iterator end, LessThan&& less_than, Equal&& equal_to)
{
	if (begin == end)
		return end;
	assert(begin < end);
	Iterator i = std::prev(end);

	while (i > begin && less_than(*i))
		--i;

	return equal_to(*i) ? i : end;
}

template <typename Iterator, typename T, typename LessThan, typename Equal>
constexpr Iterator breaking_linear_search(Iterator begin, Iterator end, T&& key, LessThan&& less_than, Equal&& equal_to)
{
	return ::breaking_linear_search_base(begin, end,
		[&](auto& right) { return less_than(key, right); },
		[&](auto& right) { return equal_to(key, right); });
}

template <typename Iterator, typename T>
constexpr Iterator breaking_linear_search(Iterator begin, Iterator end, T&& key)
{
	return ::breaking_linear_search_base(begin, end,
		[&](auto& right) { return key >= right; },
		[&](auto& right) { return key == right; });
}

template <typename Collection, typename T>
constexpr auto breaking_linear_search(Collection&& collection, T&& key)
{
	return ::breaking_linear_search(collection.begin(), collection.end(), std::forward<T>(key));
}



template <typename Iterator, typename LessThan, typename Equal>
constexpr Iterator standard_binary_search_base(Iterator begin, Iterator end, LessThan&& less_than, Equal&& equal_to)
{
	if (begin == end)
		return end;
	assert(begin < end);
	Iterator low = begin;
	Iterator high = std::prev(end);
	while (low < high)
	{
		Iterator mid = std::next(low, std::distance(low, high) / 2);
		if (less_than(*mid))
			high = std::prev(mid);
		else
			low = mid;
	}
	return equal_to(*high) ? high : end;
}

template <typename Iterator, typename T, typename LessThan, typename Equal>
constexpr Iterator standard_binary_search(Iterator begin, Iterator end, T&& key, LessThan&& less_than, Equal&& equal_to)
{
	return ::standard_binary_search_base(begin, end,
		[&](auto& right) { return less_than(key, right); },
		[&](auto& right) { return equal_to(key, right); });
}

template <typename Iterator, typename T>
constexpr Iterator standard_binary_search(Iterator begin, Iterator end, T&& key)
{
	return ::standard_binary_search_base(begin, end,
		[&](auto& right) { return key < right; },
		[&](auto& right) { return key == right; });
}

template <typename Collection, typename T>
constexpr auto standard_binary_search(Collection&& collection, T&& key)
{
	return ::standard_binary_search(collection.begin(), collection.end(), std::forward<T>(key));
}



template <typename Iterator, typename LessThan, typename Equal>
constexpr Iterator boundless_binary_search_base(Iterator begin, Iterator end, LessThan&& less_than, Equal&& equal_to)
{
	if (begin == end)
		return end;
	assert(begin < end);
	auto mid = std::distance(begin, end);
	decltype(mid) bot = 0;

	while (mid > 1)
	{
		if (!less_than(*std::next(begin, bot + mid / 2)))
			bot += mid++ / 2;
		mid /= 2;
	}

	Iterator target = std::next(begin, bot);
	return equal_to(*target) ? target : end;
}

template <typename Iterator, typename T, typename LessThan, typename Equal>
constexpr Iterator boundless_binary_search(Iterator begin, Iterator end, T&& key, LessThan&& less_than, Equal&& equal_to)
{
	return ::boundless_binary_search_base(begin, end,
		[&](auto& right) { return less_than(key, right); },
		[&](auto& right) { return equal_to(key, right); });
}

template <typename Iterator, typename T>
constexpr Iterator boundless_binary_search(Iterator begin, Iterator end, T&& key)
{
	return ::boundless_binary_search_base(begin, end,
		[&](auto& right) { return key >= right; },
		[&](auto& right) { return key == right; });
}

template <typename Collection, typename T>
constexpr auto boundless_binary_search(Collection&& collection, T&& key)
{
	return ::boundless_binary_search(collection.begin(), collection.end(), std::forward<T>(key));
}



template <typename Iterator, typename LessThan, typename Equal>
constexpr Iterator doubletapped_binary_search_base(Iterator begin, Iterator end, LessThan&& less_than, Equal&& equal_to)
{
	assert(begin < end);
	auto mid = std::distance(begin, end);
	decltype(mid) bot = 0;

	while (mid > 2)
	{
		if (!less_than(*std::next(begin, bot + mid / 2)))
			bot += mid++ / 2;
		mid /= 2;
	}

	while (mid--)
	{
		Iterator target = std::next(begin, bot + mid);
		if (equal_to(*target))
			return target;
	}

	return end;
}

template <typename Iterator, typename T, typename LessThan, typename Equal>
constexpr Iterator doubletapped_binary_search(Iterator begin, Iterator end, T&& key, LessThan&& less_than, Equal&& equal_to)
{
	return ::doubletapped_binary_search_base(begin, end,
		[&](auto& right) { return less_than(key, right); },
		[&](auto& right) { return equal_to(key, right); });
}

template <typename Iterator, typename T>
constexpr Iterator doubletapped_binary_search(Iterator begin, Iterator end, T&& key)
{
	return ::doubletapped_binary_search_base(begin, end,
		[&](auto& right) { return key >= right; },
		[&](auto& right) { return key == right; });
}

template <typename Collection, typename T>
constexpr auto doubletapped_binary_search(Collection&& collection, T&& key)
{
	return ::doubletapped_binary_search(collection.begin(), collection.end(), std::forward<T>(key));
}



template <typename Iterator, typename LessThan, typename Equal>
constexpr Iterator monobound_binary_search_base(Iterator begin, Iterator end, LessThan&& less_than, Equal&& equal_to)
{
	if (begin == end)
		return end;
	assert(begin < end);
	Iterator low = begin;
	Iterator high = end;
	const Iterator second = std::next(begin);

	while (high > second)
	{
		const auto k = std::distance(begin, high) / 2;
		if (!less_than(*std::next(begin, k)))
			low += k;
		high -= k;
	}

	return equal_to(*low) ? low : end;
}

template <typename Iterator, typename T, typename LessThan, typename Equal>
constexpr Iterator monobound_binary_search(Iterator begin, Iterator end, T&& key, LessThan&& less_than, Equal&& equal_to)
{
	return ::monobound_binary_search_base(begin, end,
		[&](auto& right) { return less_than(key, right); },
		[&](auto& right) { return equal_to(key, right); });
}

template <typename Iterator, typename T>
constexpr Iterator monobound_binary_search(Iterator begin, Iterator end, T&& key)
{
	return ::monobound_binary_search_base(begin, end,
		[&](auto& right) { return key < right; },
		[&](auto& right) { return key == right; });
}

template <typename Collection, typename T>
constexpr auto monobound_binary_search(Collection&& collection, T&& key)
{
	return ::monobound_binary_search(collection.begin(), collection.end(), std::forward<T>(key));
}



template <typename Iterator, typename LessThan, typename Equal>
constexpr Iterator tripletapped_binary_search_base(Iterator begin, Iterator end, LessThan&& less_than, Equal&& equal_to)
{
	if (begin == end)
		return end;

	assert(begin < end);

	auto top = std::distance(begin, end);
	decltype(top) bot = 0;
	decltype(top) mid = 0;

	while (top > 3)
	{
		auto mid = top / 2;
		if (!less_than(*std::next(begin, bot + mid)))
			bot += mid;
		top -= mid;
	}

	while (mid--)
	{
		Iterator target = std::next(begin, bot + top);
		if (equal_to(*target))
			return target;
	}

	return end;
}

template <typename Iterator, typename T, typename LessThan, typename Equal>
constexpr Iterator tripletapped_binary_search(Iterator begin, Iterator end, T&& key, LessThan&& less_than, Equal&& equal_to)
{
	return ::tripletapped_binary_search_base(begin, end,
		[&](auto& right) { return less_than(key, right); },
		[&](auto& right) { return equal_to(key, right); });
}

template <typename Iterator, typename T>
constexpr Iterator tripletapped_binary_search(Iterator begin, Iterator end, T&& key)
{
	return ::tripletapped_binary_search_base(begin, end,
		[&](auto& right) { return key < right; },
		[&](auto& right) { return key == right; });
}

template <typename Collection, typename T>
constexpr auto tripletapped_binary_search(Collection&& collection, T&& key)
{
	return ::tripletapped_binary_search(collection.begin(), collection.end(), std::forward<T>(key));
}



template <typename Iterator, typename LessThan, typename Equal>
constexpr Iterator monobound_quaternary_search_base(Iterator begin, Iterator end, LessThan&& less_than, Equal&& equal_to)
{
	if (begin == end)
		return end;

	assert(begin < end);

	auto top = std::distance(begin, end);
	decltype(top) bot = 0;

	while (top >= 65536)
	{
		auto mid = top / 4;
		top -= mid * 3;
		if (less_than(*std::next(begin, bot + mid * 2)))
		{
			if (!less_than(*std::next(begin, bot + mid)))
				bot += mid;
		}
		else
		{
			bot += mid * 2;
			if (!less_than(*std::next(begin, bot + mid)))
				bot += mid;
		}
	}

	while (top > 3)
	{
		auto mid = top / 2;
		if (!less_than(*std::next(begin, bot + mid)))
			bot += mid;
		top -= mid;
	}

	while (top--)
	{
		auto target = std::next(begin, bot + top);
		if (equal_to(*target))
			return target;
	}

	return end;
}

template <typename Iterator, typename T, typename LessThan, typename Equal>
constexpr Iterator monobound_quaternary_search(Iterator begin, Iterator end, T&& key, LessThan&& less_than, Equal&& equal_to)
{
	return ::monobound_quaternary_search_base(begin, end,
		[&](auto& right) { return less_than(key, right); },
		[&](auto& right) { return equal_to(key, right); });
}

template <typename Iterator, typename T>
constexpr Iterator monobound_quaternary_search(Iterator begin, Iterator end, T&& key)
{
	return ::monobound_quaternary_search_base(begin, end,
		[&](auto& right) { return key < right; },
		[&](auto& right) { return key == right; });
}

template <typename Collection, typename T>
constexpr auto monobound_quaternary_search(Collection&& collection, T&& key)
{
	return ::monobound_quaternary_search(collection.begin(), collection.end(), std::forward<T>(key));
}



template <typename Iterator, typename T, typename LessThan, typename Equal>
constexpr Iterator monobound_interpolated_search_base(Iterator begin, Iterator end, T&& key, LessThan&& less_than, Equal&& equal_to)
{
	if (begin == end)
		return end;

	assert(begin < end);

	if (less_than(*begin))
		return end;

	auto size = std::distance(begin, end);
	auto bot = size - 1;
	auto max = std::next(begin, bot);

	if (!less_than(*max))
		return equal_to(max) ? max : end;

	auto min = begin;
	using real_type = std::conditional_t<(sizeof(bot) > 4), double, float>;
	bot *= (real_type)(key - *min) / (*max - *min);
	auto top = 64;

	if (!less_than(std::next(begin, bot)))
	{
		while (true)
		{
			if (bot + top >= size)
			{
				top = size - bot;
				break;
			}
			bot += top;
			if (less_than(*std::next(begin, bot)))
			{
				bot -= top;
				break;
			}
			top *= 2;
		}
	}
	else
	{
		while (true)
		{
			if (bot < top)
			{
				top = bot;
				bot = 0;
				break;
			}
			bot -= top;
			if (less_than(*std::next(begin, bot)))
				break;
			top *= 2;
		}
	}

	while (top > 3)
	{
		auto mid = top / 2;
		if (!less_than(*std::next(begin, bot + mid)))
			bot += mid;
		top -= mid;
	}

	while (top--)
	{
		auto target = std::next(begin, bot + top);
		if (equal_to(*target))
			return target;
	}

	return end;
}

template <typename Iterator, typename T, typename LessThan, typename Equal>
constexpr Iterator monobound_interpolated_search(Iterator begin, Iterator end, T&& key, LessThan&& less_than, Equal&& equal_to)
{
	return ::monobound_interpolated_search_base(begin, end, key,
		[&](auto& right) { return less_than(key, right); },
		[&](auto& right) { return equal_to(key, right); });
}

template <typename Iterator, typename T>
constexpr Iterator monobound_interpolated_search(Iterator begin, Iterator end, T&& key)
{
	return ::monobound_interpolated_search_base(begin, end, key,
		[&](auto& right) { return key < right; },
		[&](auto& right) { return key == right; });
}

template <typename Collection, typename T>
constexpr auto monobound_interpolated_search(Collection&& collection, T&& key)
{
	return ::monobound_interpolated_search(collection.begin(), collection.end(), std::forward<T>(key));
}




struct adaptive_binary_search_state
{
	size_t i, balance;
};

template <typename Iterator, typename T, typename LessThan, typename Equal>
constexpr Iterator adaptive_binary_search_base(Iterator begin, Iterator end, T&& key, LessThan&& less_than, Equal&& equal_to, adaptive_binary_search_state& state)
{
	if (begin == end)
		return end;
	assert(begin < end);
	auto size = std::distance(begin, end);
	decltype(size) bot, top;

	if (state.balance < 32 && size > 64)
	{
		bot = state.i;
		top = 32;

		if (!less_than(*std::next(begin, bot)))
		{
			while (true)
			{
				if (bot + top >= size)
				{
					top = size - bot;
					break;
				}
				bot += top;
				if (less_than(*std::next(begin, bot)))
				{
					bot -= top;
					break;
				}
				top *= 2;
			}
		}
		else
		{
			while (true)
			{
				if (bot < top)
				{
					top = bot;
					bot = 0;
					break;
				}
				bot -= top;
				if (!less_than(*std::next(begin, bot)))
					break;
				top *= 2;
			}
		}
	}
	else
	{
		bot = 0;
		top = array_size;
	}

	while (top != 0)
		if (equal_to(*std::next(begin, bot + --top)))
			return std::next(begin, bot + top);

	return end;
}

template <typename Iterator, typename T, typename LessThan, typename Equal>
constexpr Iterator adaptive_binary_search(Iterator begin, Iterator end, T&& key, LessThan&& less_than, Equal&& equal_to, adaptive_binary_search_state& state)
{
	return ::adaptive_binary_search_base(begin, end, key,
		[&](auto& right) { return less_than(key, right); },
		[&](auto& right) { return equal_to(key, right); },
		state);
}

template <typename Iterator, typename T>
constexpr Iterator adaptive_binary_search(Iterator begin, Iterator end, T&& key, adaptive_binary_search_state& state)
{
	return ::adaptive_binary_search_base(begin, end, key,
		[&](auto& right) { return key < right; },
		[&](auto& right) { return key == right; },
		state);
}

template <typename Collection, typename T>
constexpr auto adaptive_binary_search(Collection&& collection, T&& key, adaptive_binary_search_state& state)
{
	return ::adaptive_binary_search(collection.begin(), collection.end(), std::forward<T>(key), state);
}

#endif