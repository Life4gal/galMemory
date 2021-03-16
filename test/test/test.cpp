#include <gtest/gtest.h>

#include <basic_value_type.hpp>

namespace {
	template<size_t... N>
	static constexpr auto square_nums(size_t index, std::index_sequence<N...>) {
		constexpr auto nums = std::array{N * N...};
		return nums[index];
	}

	template<size_t N>
	constexpr static auto const_nums(size_t index) {
		return square_nums(index, std::make_index_sequence<N>{});
	}

	template<typename T, T... ints>
	void print_sequence(std::integer_sequence<T, ints...> int_seq) {
		std::cout << "The sequence of size " << int_seq.size() << ": ";
		((std::cout << ints << ' '), ...);
		std::cout << '\n';
	}

	template<typename Array, std::size_t... I>
	auto a2t_impl(const Array& a, std::index_sequence<I...>) {
		return std::make_tuple(a[I]...);
	}

	template<typename T, std::size_t N, typename Indices = std::make_index_sequence<N>>
	auto a2t(const std::array<T, N>& a) {
		return a2t_impl(a, Indices{});
	}

	template<class Ch, class Tr, class Tuple, std::size_t... Is>
	void print_tuple_impl(std::basic_ostream<Ch, Tr>& os,
						  const Tuple& t,
						  std::index_sequence<Is...>) {
		((os << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
	}

	template<class Ch, class Tr, class... Args>
	auto& operator<<(std::basic_ostream<Ch, Tr>& os, const std::tuple<Args...>& t) {
		os << "(";
		print_tuple_impl(os, t, std::index_sequence_for<Args...>{});
		return os << ")";
	}

	TEST(TestHello, just_do_it) {
		static_assert(const_nums<101>(100) == 100 * 100);

		print_sequence(std::integer_sequence<unsigned, 9, 2, 5, 1, 9, 1, 6>{});
		print_sequence(std::make_integer_sequence<int, 20>{});
		print_sequence(std::make_index_sequence<10>{});
		print_sequence(std::index_sequence_for<float, std::iostream, char>{});

		std::array<int, 4> array = {1, 2, 3, 4};

		auto tuple = a2t(array);
		static_assert(std::is_same<decltype(tuple),
								   std::tuple<int, int, int, int>>::value);

		std::cout << tuple << '\n';
	}

	TEST(basic_value_type, test_basic_value_type) {
		using namespace gal::memory;

		ASSERT_EQ(OneByteValueMax, std::numeric_limits<std::uint8_t>::max());
		ASSERT_EQ(TwoByteValueMax, std::numeric_limits<std::uint16_t>::max());
		ASSERT_EQ(FourByteValueMax, std::numeric_limits<std::uint32_t>::max());
		ASSERT_EQ(EightByteValueMax, std::numeric_limits<std::uint64_t>::max());

		ASSERT_EQ(0xff, static_cast<std::underlying_type_t<wildcard_type>>(wildcard_type::FIXED));
		ASSERT_EQ(0x00, static_cast<std::underlying_type_t<wildcard_type>>(wildcard_type::WILDCARD));

		/*
		OneByteValue one{-42};
		TwoByteValue two{-42};
		FourByteValue four{-42};
		EightByteValue eight{-42};

		ASSERT_EQ(one.to_signed(), -42);
		ASSERT_EQ(one.to_unsigned(), OneByteValue::unsigned_max + (-42) + 1);
		ASSERT_EQ(two.to_signed(), -42);
		ASSERT_EQ(two.to_unsigned(), TwoByteValue::unsigned_max + (-42) + 1);
		ASSERT_EQ(four.to_signed(), -42);
		ASSERT_EQ(four.to_unsigned(), FourByteValue::unsigned_max + (-42) + 1);
		ASSERT_EQ(eight.to_signed(), -42);
		ASSERT_EQ(eight.to_unsigned(), EightByteValue::unsigned_max + (-42) + 1);

		EightByteFloatValue d{3.14};
		ASSERT_DOUBLE_EQ(d.to_64bit(), 3.14);
		ASSERT_FLOAT_EQ(d.to_32bit(), 3.14f);
		 */

		std::cout << OneByteValue{}.size() << ' ' << TwoByteValue{}.size() << ' ' << FourByteValue{}.size() << ' ' << EightByteValue{}.size() << std::endl;
	}


}// namespace
