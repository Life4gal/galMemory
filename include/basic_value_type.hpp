#ifndef GALMEMORY_BASIC_VALUE_TYPE_HPP
#define GALMEMORY_BASIC_VALUE_TYPE_HPP

#include <bitset>
#include <cstring>
#include <numeric>
#include <type_traits>
#include <variant>

namespace gal::memory {
	inline namespace support {
		template<size_t Bytes>
		using BitsetValue = std::bitset<std::numeric_limits<std::underlying_type_t<std::byte>>::digits * Bytes>;

		// use unsigned type store the value
		using OneByteValue = BitsetValue<1>;
		using TwoByteValue = BitsetValue<2>;
		using FourByteValue = BitsetValue<3>;
		using EightByteValue = BitsetValue<4>;

		constexpr unsigned char BitsetValueMin = std::numeric_limits<std::underlying_type_t<std::byte>>::min();
		constexpr unsigned char OneByteValueMax = std::numeric_limits<std::underlying_type_t<std::byte>>::max();
		constexpr unsigned short TwoByteValueMax = (static_cast<unsigned short>(OneByteValueMax) << sizeof(std::byte) * 8 * 1) + OneByteValueMax;
		constexpr unsigned int FourByteValueMax = (static_cast<unsigned int>(TwoByteValueMax) << sizeof(std::byte) * 8 * 2) + TwoByteValueMax;
		constexpr unsigned long EightByteValueMax = (static_cast<unsigned long>(FourByteValueMax) << sizeof(std::byte) * 8 * 4) + FourByteValueMax;

		/*
		struct OneByteValue {
			std::int8_t value;

			constexpr static auto signed_max = std::numeric_limits<std::int8_t>::max();
			constexpr static auto signed_min = std::numeric_limits<std::int8_t>::min();
			constexpr static auto unsigned_max = std::numeric_limits<std::uint8_t>::max();
			constexpr static auto unsigned_min = std::numeric_limits<std::uint8_t>::min();

			[[nodiscard]] std::int8_t to_signed() const noexcept {
				return static_cast<std::int8_t>(value);
			}
			[[nodiscard]] std::uint8_t to_unsigned() const noexcept {
				return static_cast<std::uint8_t>(value);
			}

			[[nodiscard]] std::string to_string() const {
				return std::to_string(value);
			}
		};
		struct TwoByteValue {
			std::int16_t value;

			constexpr static auto signed_max = std::numeric_limits<std::int16_t>::max();
			constexpr static auto signed_min = std::numeric_limits<std::int16_t>::min();
			constexpr static auto unsigned_max = std::numeric_limits<std::uint16_t>::max();
			constexpr static auto unsigned_min = std::numeric_limits<std::uint16_t>::min();

			[[nodiscard]] std::int16_t to_signed() const noexcept {
				return static_cast<std::int16_t>(value);
			}
			[[nodiscard]] std::uint16_t to_unsigned() const noexcept {
				return static_cast<std::uint16_t>(value);
			}

			[[nodiscard]] std::string to_string() const {
				return std::to_string(value);
			}
		};
		struct FourByteValue {
			std::int32_t value;

			constexpr static auto signed_max = std::numeric_limits<std::int32_t>::max();
			constexpr static auto signed_min = std::numeric_limits<std::int32_t>::min();
			constexpr static auto unsigned_max = std::numeric_limits<std::uint32_t>::max();
			constexpr static auto unsigned_min = std::numeric_limits<std::uint32_t>::min();

			[[nodiscard]] std::int32_t to_signed() const noexcept {
				return static_cast<std::int32_t>(value);
			}
			[[nodiscard]] std::uint32_t to_unsigned() const noexcept {
				return static_cast<std::uint32_t>(value);
			}

			[[nodiscard]] std::string to_string() const {
				return std::to_string(value);
			}
		};
		struct EightByteValue {
			std::int64_t value;

			constexpr static auto signed_max = std::numeric_limits<std::int64_t>::max();
			constexpr static auto signed_min = std::numeric_limits<std::int64_t>::min();
			constexpr static auto unsigned_max = std::numeric_limits<std::uint64_t>::max();
			constexpr static auto unsigned_min = std::numeric_limits<std::uint64_t>::min();

			[[nodiscard]] std::int64_t to_signed() const noexcept {
				return static_cast<std::int64_t>(value);
			}
			[[nodiscard]] std::uint64_t to_unsigned() const noexcept {
				return static_cast<std::uint64_t>(value);
			}

			[[nodiscard]] std::string to_string() const {
				return std::to_string(value);
			}
		};
		struct EightByteFloatValue {
			double value;

			constexpr static auto bit64_max = std::numeric_limits<double>::max();
			constexpr static auto bit64_min = std::numeric_limits<double>::min();
			constexpr static auto bit32_max = std::numeric_limits<float>::max();
			constexpr static auto bit32_min = std::numeric_limits<float>::min();

			[[nodiscard]] double to_64bit() const noexcept {
				return static_cast<double>(value);
			}
			[[nodiscard]] float to_32bit() const noexcept {
				return static_cast<float>(value);
			}

			[[nodiscard]] std::string to_string() const {
				return std::to_string(value);
			}
		};
		 */
	}// namespace support

	enum class wildcard_type : std::underlying_type_t<std::byte> {
		WILDCARD = std::numeric_limits<std::underlying_type_t<wildcard_type>>::min(),
		FIXED = std::numeric_limits<std::underlying_type_t<wildcard_type>>::max()
	};

	static_assert(sizeof(wildcard_type) == sizeof(std::byte));

	// std::variant default construct use the first type(index 0) to init
	using memory_type = std::variant<
			EightByteValue,
			FourByteValue,
			TwoByteValue,
			OneByteValue>;

	struct basic_value_type {
		memory_type value;

		[[nodiscard]] std::string to_string() const {
			return std::visit([](auto val) { return val.to_string(); }, value);
		}

		[[nodiscard]] auto to_ulong() const {
			return std::visit([](auto val) { return val.to_ulong(); }, value);
		}

		[[nodiscard]] auto to_ullong() const {
			return std::visit([](auto val) { return val.to_ullong(); }, value);
		}
	};

	struct advance_value_type {
		basic_value_type basic_value;

		std::shared_ptr<std::byte[]> bytes;
		std::shared_ptr<wildcard_type[]> wildcard_value;

		explicit advance_value_type(basic_value_type basic_value = {}) : basic_value(basic_value),
																		 bytes(nullptr),
																		 wildcard_value(nullptr) {}

		void set_advance_value(char* const* argv, size_t argc) {
			bytes.reset(new std::byte[argc]);
			wildcard_value.reset(new wildcard_type[argc]);
			for (auto i = 0; i < argc; ++i) {
				/* get current string */
				auto curr_str = argv[i];
				/* test its length */
				if (!curr_str[0] || !curr_str[1] || !curr_str[2]) {
					// invalid str
					bytes.reset();
					wildcard_value.reset();
					return;
				}

				if (std::strcmp(curr_str, "??") == 0) {
					wildcard_value[i] = wildcard_type::WILDCARD;
					bytes[i] = static_cast<std::byte>(wildcard_type::WILDCARD);
				} else {
					// parse as hex integer
					char* end;
					auto curr_byte = std::strtoul(curr_str, &end, 16);
					if (*end != '\0') {
						// invalid str
						bytes.reset();
						wildcard_value.reset();
						return;
					}

					wildcard_value[i] = wildcard_type::FIXED;
					bytes[i] = static_cast<std::byte>(curr_byte);
				}
			}
		}

		void set_basic_value(const char* ptr) {
			if (ptr == nullptr) {
				return;
			}

			char* end;
			auto value = std::strtoul(ptr, &end, 0);

			if (BitsetValueMin <= value && value <= EightByteValueMax) {
				if (value >= FourByteValueMax) {
					basic_value.value = static_cast<EightByteValue>(value);
				} else if (value >= TwoByteValueMax) {
					basic_value.value = static_cast<FourByteValue>(value);
				} else if (value >= OneByteValueMax) {
					basic_value.value = static_cast<TwoByteValue>(value);
				} else {
					basic_value.value = static_cast<OneByteValue>(value);
				}
			} else {
				throw std::runtime_error("advance_value_type::set_basic_value error: invalid integral.");
			}
		}

		explicit operator basic_value_type() const {
			return {basic_value};
		}
	};

}// namespace gal::memory

#endif//GALMEMORY_BASIC_VALUE_TYPE_HPP
