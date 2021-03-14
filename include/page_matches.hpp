#ifndef GALMEMORY_PAGE_MATCHES_HPP
#define GALMEMORY_PAGE_MATCHES_HPP

#include <exception>// for std::exception
#include <numeric>	// for std::accumulate
#include <vector>	// for std::vector

#include <support.hpp>

namespace gal::memory {
	using offset_type = unsigned int;
	using offset_type_array = std::vector<offset_type>;

	class not_matches : std::exception {
		[[nodiscard]] const char* what() const noexcept override {
			return "not page matches.";
		}
	};

	class PageMatches {
	public:
		PageMatches(MemoryPage page, const offset_type_array& offsets)
			: page(page),
			  offsets(offsets) {
			if (offsets.empty()) throw not_matches{};
		}

		[[nodiscard]] const MemoryPage& get_page() const {
			return page;
		}

		[[nodiscard]] const offset_type_array& get_offset_array() const {
			return offsets;
		}

	private:
		MemoryPage page;
		offset_type_array offsets;
	};

	using matches_page_array = std::vector<PageMatches>;

	class MatchesHolder {
	public:
		using size_type = matches_page_array::size_type;

		[[nodiscard]] const matches_page_array& get_page_array() const {
			return matches;
		}

		void add(MemoryPage page, const offset_type_array& offsets) {
			if (!offsets.empty()) {
				matches.emplace_back(page, offsets);
			}
		}

		[[nodiscard]] matches_page_array::size_type total_matches() const {
			return std::accumulate(
					matches.begin(), matches.end(),
					size_type{},
					[](size_type total, const PageMatches& curr) {
						return total + curr.get_offset_array().size();
					});
		}

		[[nodiscard]] bool any() const {
			return total_matches() > 0;
		}

	private:
		matches_page_array matches;
	};

}// namespace gal::memory

#endif//GALMEMORY_PAGE_MATCHES_HPP
