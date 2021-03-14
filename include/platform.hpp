#ifndef GALMEMORY_PLATFORM_HPP
#define GALMEMORY_PLATFORM_HPP

namespace gal::memory {
#ifdef _WIN32
#include <Windows.h>
	using pid_t = DWORD;
	using phandle_t = HANDLE;
#endif

#ifdef LINUX
#include <sys/types.h>
	using phandle_t = int;
#endif
}// namespace gal::memory

#endif//GALMEMORY_PLATFORM_HPP
