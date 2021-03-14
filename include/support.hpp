#ifndef GALMEMORY_SUPPORT_HPP
#define GALMEMORY_SUPPORT_HPP

#include <functional> // for std::function
#include <iosfwd>	  // for std::istream, std::stringstream, std::cin, std::cout, std::cerr
#include <string>	  // for std::string
#include <string_view>// for std::string_vie

namespace gal::memory {
	inline namespace support {

		using byte_type = std::int8_t;
		using memory_type = std::vector<byte_type>;
		using address_type = byte_type*;
		using size_type = memory_type::size_type;

		struct MemoryPage {
			address_type start;
			size_type size;

			MemoryPage(address_type start, size_type size) : start(start), size(size) {}
		};

		template<typename T>
		using Validator = std::function<bool(const T&)>;

		template<typename T>
		using Reader = std::function<std::istream&(std::istream&, T&)>;

		template<typename T>
		constexpr Validator<T> DefaultValidator() {
			return [](const T&) { return true; };
		}

		template<typename T>
		constexpr Reader<T> DefaultReader() {
			return [](std::istream& in, T& t) -> std::istream& { return in >> t; };
		}

		template<typename T>
		inline T query_for(std::string_view message, std::string_view error, Validator<T> validator = DefaultValidator<T>(), Reader<T> reader = DefaultReader<T>());

		inline bool query_boolean(std::string_view message) {
			std::stringstream query;
			query << message << " (y/n)";
			const auto choice = query_for<char>(query.str(), "invalid choice.", [](char c) -> bool {auto d = std::tolower(c); return d == 'y' || d == 'n'; });

			return tolower(choice) == 'y';
		}

		template<typename T>
		inline T query_for(std::string_view message, std::string_view error, Validator<T> validator, Reader<T> reader) {
			constexpr static auto PROMPT = ": ";
			std::cout << message << PROMPT;
			T t;
			while (!reader(std::cin, t) || !validator(t)) {
				std::cerr << error << std::endl;
				std::cout << message << PROMPT;

				if (!std::cin) {
					std::cin.clear();
					std::cin.ignore(std::cin.rdbuf()->in_avail());
				}
			}

			return t;
		}
	}// namespace support
}// namespace gal::memory


#endif//GALMEMORY_SUPPORT_HPP
