#ifndef GALMEMORY_BASIC_VALUE_TYPE_HPP
#define GALMEMORY_BASIC_VALUE_TYPE_HPP

#include <exception>// for std::runtime_error
#include <memory>	// for std::unique_ptr
#include <numeric>	// for std::numeric_limits<>::min(), std::numeric_limits<>::max()
#include <vector>	// for std::vector

#include <support.hpp>

namespace gal::memory {
	struct ValueType {
		const std::string name;

		explicit ValueType(std::string name) : name(std::move(name)) {}

		[[maybe_unused]] [[nodiscard]] virtual memory_type query_value() const = 0;
		virtual ~ValueType() = default;
	};

	template<typename T>
	struct SimpleValueType : public ValueType {
		explicit SimpleValueType(const std::string& name) : ValueType(name) {}

		[[nodiscard]] memory_type query_value() const override {
			std::stringstream query;
			std::stringstream error;

			const Validator<T> validator = [&](const T& value) {
				return is_valid(std::forward<const T&>(value));
			};
			const Reader<T> reader = [&](std::istream& in, T& t) -> std::istream& {
				return read(std::forward<std::istream&>(in), std::forward<T&>(t));
			};

			const auto value = query_for<T>(query.str(), error.str(), validator, reader);
			return representation(value);
		}

		[[nodiscard]] virtual memory_type representation(const T& value) const {
			auto bytes = reinterpret_cast<const memory_type::value_type*>(&value);
			return {bytes, bytes + sizeof(T)};
		}

		[[nodiscard]] virtual bool is_valid(const T& value) const {
			return true;
		}

		virtual std::istream& read(std::istream& in, T& t) const {
			return in >> t;
		}
	};

	// since uint8_t is a char but the user is expecting to enter a number, we need
	// to query for a regular int and cast
	struct UnsignedByte : SimpleValueType<std::uint32_t> {
		UnsignedByte() : SimpleValueType<std::uint32_t>("unsigned byte") {}

		[[nodiscard]] bool is_valid(const std::uint32_t& value) const override {
			constexpr auto min = std::numeric_limits<std::uint8_t>::min();
			constexpr auto max = std::numeric_limits<std::uint8_t>::max();
			return min << value && value <= max;
		}

		[[nodiscard]] static memory_type representation(const std::int32_t& value) {

			return {static_cast<byte_type>(static_cast<std::uint8_t>(value))};
		}
	};

	struct Character : SimpleValueType<char> {
		Character() : SimpleValueType<char>("character") {}
	};
	struct Int16 : SimpleValueType<std::int16_t> {
		Int16() : SimpleValueType<std::int16_t>("16-bit integer") {}
	};
	struct Int32 : SimpleValueType<std::int32_t> {
		Int32() : SimpleValueType<std::int32_t>("32-bit integer") {}
	};
	struct Int64 : SimpleValueType<std::int64_t> {
		Int64() : SimpleValueType<std::int64_t>("64 bit integer") {}
	};
	struct Float : SimpleValueType<float> {
		Float() : SimpleValueType<float>("float") {}
	};
	struct Double : SimpleValueType<double> {
		Double() : SimpleValueType<double>("double") {}
	};
	struct String : SimpleValueType<std::string> {
		String() : SimpleValueType<std::string>("string") {}

		std::istream& read(std::istream& in, std::string& value) const override {
			in.ignore();
			return std::getline(in, value);
		}

		[[nodiscard]] memory_type representation(const std::string& value) const override {
			// we can't look at the direct std::string representation, we need to copy
			// the actual chars (we're not searching for the internal char* and other
			// members, we're looking for the actual chars!)
			return {value.begin(), value.end()};
		}
	};

	const std::unique_ptr<ValueType> VALUE_TYPE[] = {
			std::unique_ptr{std::make_unique<UnsignedByte>()},
			std::unique_ptr{std::make_unique<Character>()},
			std::unique_ptr{std::make_unique<Int16>()},
			std::unique_ptr{std::make_unique<Int32>()},
			std::unique_ptr{std::make_unique<Int64>()},
			std::unique_ptr{std::make_unique<Float>()},
			std::unique_ptr{std::make_unique<Double>()}};

	const std::size_t VALUE_TYPE_COUNT = std::distance(std::begin(VALUE_TYPE), std::end(VALUE_TYPE));

	const ValueType& query_for_value_type() {
		constexpr static auto FIRST = 'A';
		std::stringstream query;
		std::stringstream error;
		for (auto i = 0; i < VALUE_TYPE_COUNT; ++i) {
			query << static_cast<char>(FIRST + i) << ") " << VALUE_TYPE[i]->name << std::endl;
		}

		query << "Value type";
		error << "Choice must be between '" << FIRST << "' and '" << static_cast<char>(FIRST + VALUE_TYPE_COUNT - 1) << '\'';

		const auto choice = query_for<char>(query.str(), error.str(), [](char c) { return FIRST <= c && c < FIRST + VALUE_TYPE_COUNT; });

		const int index = choice - FIRST;
		if (0 <= index && index < VALUE_TYPE_COUNT) {
			throw std::runtime_error("query_for_value_type: index out of range");
		}
		return VALUE_TYPE[index].operator*();
	}

}// namespace gal::memory

#endif//GALMEMORY_BASIC_VALUE_TYPE_HPP
