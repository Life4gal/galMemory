#ifndef GALMEMORY_PROCESS_HPP
#define GALMEMORY_PROCESS_HPP

#include <platform.hpp>
#include <support.hpp>
#include <vector>// for std::vector

namespace gal::memory {
	using memory_page_array = std::vector<MemoryPage>;

	class Process {
	public:
		explicit Process(pid_t pid) : pid(pid) {}

		[[nodiscard]] virtual memory_page_array get_memory_page() const = 0;

		[[nodiscard]] virtual memory_type read(MemoryPage page) = 0;

		virtual void write(address_type address, const memory_type& value) = 0;

		virtual ~Process() = default;

	protected:
		pid_t pid;
	};
}// namespace gal::memory

#endif//GALMEMORY_PROCESS_HPP
