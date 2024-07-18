#pragma once
#include <windows.h>
#include <mutex>
#include <cstdint>
#include <map>

#include "util/util.hpp"
#include "loadup.hpp"
#include "raw_driver.hpp"

#pragma pack ( push, 1 )
typedef struct _GIOMAP
{
	unsigned long	interface_type;
	unsigned long	bus;
	std::uintptr_t  physical_address;
	unsigned long	io_space;
	unsigned long	size;
} GIOMAP;
#pragma pack ( pop )

namespace physmeme
{
	inline std::string drv_key;

	//
	// please code this function depending on your method of physical read/write.
	//
	inline HANDLE load_drv()
	{
		const auto [result, key] = driver::load(raw_driver, sizeof(raw_driver));
		drv_key = key;

		return CreateFile(
			"\\\\.\\GIO",
			GENERIC_READ | GENERIC_WRITE,
			NULL,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
	}

	//
	// please code this function depending on your method of physical read/write.
	//
	inline bool unload_drv()
	{
		return driver::unload(drv_key);
	}

	inline HANDLE drv_handle = load_drv();

	//
	// please code this function depending on your method of physical read/write.
	//
	inline std::uintptr_t map_phys(
		std::uintptr_t addr,
		std::size_t size
	)
	{
		//--- ensure the validity of the address we are going to try and map
		if (!util::is_valid(addr))
			return NULL;

		GIOMAP in_buffer = { 0, 0, addr, 0, size };
		uintptr_t out_buffer[2] = { 0 };
		unsigned long returned = 0;
		DeviceIoControl(drv_handle, 0xC3502004, reinterpret_cast<LPVOID>(&in_buffer), sizeof(in_buffer),
			reinterpret_cast<LPVOID>(out_buffer), sizeof(out_buffer), &returned, NULL);
		return out_buffer[0];

	}

	//
	// please code this function depending on your method of physical read/write.
	//
	inline bool unmap_phys(
		std::uintptr_t addr,
		std::size_t size
	)
	{
		uintptr_t in_buffer = addr;
		uintptr_t out_buffer[2] = { sizeof(out_buffer) };

		unsigned long returned = NULL;
		DeviceIoControl(drv_handle, 0xC3502008, reinterpret_cast<LPVOID>(&in_buffer), sizeof(in_buffer),
			reinterpret_cast<LPVOID>(out_buffer), sizeof(out_buffer), &returned, NULL);
		return out_buffer[0];
	}
}
