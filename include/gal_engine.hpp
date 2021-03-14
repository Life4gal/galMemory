#ifndef GALMEMORY_ENGINE_HPP
#define GALMEMORY_ENGINE_HPP

#include <algorithm>// for std::copy_if, std::equal, std::search
#include <exception>// for std::runtime_error
#include <iterator> // for std::back_inserter
#include <page_matches.hpp>
#include <process.hpp>

namespace gal::memory {
	class GalEngine {
	public:
		explicit GalEngine(Process& process) : process(process) {}

		[[nodiscard]] MatchesHolder search(const memory_type& value) const {
			MatchesHolder matches;

			const auto pages = process.get_memory_page();

			for (const auto& page: pages) {
				const auto memory = process.read(page);
				const auto offsets = matching_offsets(memory, value);
				matches.add(page, offsets);
			}

			return matches;
		}

		[[nodiscard]] MatchesHolder narrow_down(const MatchesHolder& matches, const memory_type& value) const {
			MatchesHolder narrowed;

			const auto& pages = matches.get_page_array();
			for (const auto& match: pages) {
				const auto memory = process.read(match.get_page());

				offset_type_array offsets;
				auto& offset_array = match.get_offset_array();
				std::copy_if(
						offset_array.begin(), offset_array.end(),
						std::back_inserter(offsets),
						[&](offset_type offset) {
							if (offset + value.size() > memory.size()) {
								throw std::runtime_error("narrow_down: offset out of range");
							}
							return std::equal(value.begin(), value.end(), memory.begin() + offset);
						});

				narrowed.add(match.get_page(), offsets);
			}

			return narrowed;
		}

		void modify(const MatchesHolder& matches, const memory_type& value) {
			const auto& pages = matches.get_page_array();
			for (const auto& match: pages) {
				const auto& offsets = match.get_offset_array();
				for (auto offset: offsets) {
					const auto address = match.get_page().start + offset;
					process.write(address, value);
				}
			}
		}

		[[nodiscard]] static bool done_searching(const MatchesHolder& matches) {
			return matches.total_matches() <= 1;
		}

	private:
		Process& process;

		[[nodiscard]] static offset_type_array matching_offsets(const memory_type& haystack, const memory_type& needle) {
			offset_type_array offsets;

			for (auto it = haystack.cbegin(); (it = std::search(it, haystack.cend(), needle.cbegin(), needle.cend())) != haystack.end(); ++it) {
				const auto offset = std::distance(haystack.cbegin(), it);
				offsets.push_back(offset);
			}

			return offsets;
		}
	};
}// namespace gal::memory

#endif//GALMEMORY_ENGINE_HPP
