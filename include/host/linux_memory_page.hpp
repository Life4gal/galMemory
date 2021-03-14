#ifndef GALMEMORY_LINUX_PAGE_MATCH_HPP
#define GALMEMORY_LINUX_PAGE_MATCH_HPP

#include <exception>// for std::invalid_argument
#include <regex>	//  for std::regex, std::smatch, std::regex_match
#include <support.hpp>

namespace gal::memory {
	struct LinuxMemoryPage : public MemoryPage {
		bool read;
		bool write;
		bool shared;

		LinuxMemoryPage(address_type start, size_type size, bool read, bool write, bool shared)
			: MemoryPage(start, size),
			  read(read),
			  write(write),
			  shared(shared) {}

		[[nodiscard]] bool accessible() const {
			return read && write && !shared;
		}
	};

	struct invalid_memory_map_line : std::exception {
		[[nodiscard]] const char* what() const noexcept override {
			return "parse_memory_map_line failed";
		}
	};

	LinuxMemoryPage parse_memory_map_line(const std::string& line) {
		const static std::regex pattern{"([0-9A-Fa-f]+)-([0-9A-Fa-f]+) ([-r])([-w])[-x]([sp]).*"};

		std::smatch matches;
		std::regex_match(line, matches, pattern);

		if (matches.size() != 6) {
			throw invalid_memory_map_line{};
		}

		address_type start;
		address_type end;
		try {
			start = reinterpret_cast<address_type>(std::stoull(matches[1], nullptr, 16));
			end = reinterpret_cast<address_type>(std::stoull(matches[2], nullptr, 16));
		} catch (const std::invalid_argument&) {
			throw invalid_memory_map_line{};
		}

		const auto read = matches[3] == 'r';
		const auto write = matches[4] == 'w';
		const auto shared = matches[5] == 's';

		return LinuxMemoryPage(start, static_cast<size_type>(end - start), read, write, shared);
	}

}// namespace gal::memory

#endif//GALMEMORY_LINUX_PAGE_MATCH_HPP
