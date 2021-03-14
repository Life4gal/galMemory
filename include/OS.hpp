#ifndef GALMEMORY_OS_HPP
#define GALMEMORY_OS_HPP

#include <memory>// for std::unique_ptr
#include <process.hpp>
#include <string_view>// for std::string_view

namespace gal::memory {
	using pid_array = std::vector<pid_t>;

	struct OS {
		static std::unique_ptr<OS> get_os();

		virtual pid_array get_processes(std::string_view name) const = 0;

		virtual std::unique_ptr<Process> open(pid_t pid) = 0;

		virtual ~OS() = default;
	};
}// namespace gal::memory

#endif//GALMEMORY_OS_HPP
