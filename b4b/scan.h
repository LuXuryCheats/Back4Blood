
namespace Scanners
{
	uintptr_t PatternScan(uintptr_t pModuleBaseAddress, const char* sSignature, size_t nSelectResultIndex = 0);
	uintptr_t PatternScan(const char* sSignature, size_t nSelectResultIndex = 0);
}

uintptr_t Scanners::PatternScan(uintptr_t pModuleBaseAddress, const char* sSignature, size_t nSelectResultIndex)
{
	static auto patternToByte = [](const char* pattern)
	{
		auto       bytes = std::vector<int>{};
		const auto start = const_cast<char*>(pattern);
		const auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current)
		{
			if (*current == '?')
			{
				++current;
				if (*current == '?')
					++current;
				bytes.push_back(-1);
			}
			else {
				bytes.push_back(strtoul(current, &current, 16));
			}
		}
		return bytes;
	};

	const auto dosHeader = (PIMAGE_DOS_HEADER)pModuleBaseAddress;
	const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)pModuleBaseAddress + dosHeader->e_lfanew);

	const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	auto       patternBytes = patternToByte(sSignature);
	const auto scanBytes = reinterpret_cast<std::uint8_t*>(pModuleBaseAddress);

	const auto s = patternBytes.size();
	const auto d = patternBytes.data();

	size_t nFoundResults = 0;

	for (auto i = 0ul; i < sizeOfImage - s; ++i)
	{
		bool found = true;

		for (auto j = 0ul; j < s; ++j)
		{
			if (scanBytes[i + j] != d[j] && d[j] != -1)
			{
				found = false;
				break;
			}
		}

		if (found)
		{
			if (nSelectResultIndex != 0)
			{
				if (nFoundResults < nSelectResultIndex)
				{
					nFoundResults++;
					found = false;
				}
				else
				{
					return reinterpret_cast<uintptr_t>(&scanBytes[i]);
				}
			}
			else
			{
				return reinterpret_cast<uintptr_t>(&scanBytes[i]);
			}
		}
	}

	return NULL;
}

uintptr_t Scanners::PatternScan(const char* sSignature, size_t nSelectResultIndex)
{
	static bool bIsSetted = false;

	static MODULEINFO info = { 0 };

	if (!bIsSetted)
	{
		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &info, sizeof(info));
		bIsSetted = true;
	}

	return PatternScan((uintptr_t)info.lpBaseOfDll, sSignature, nSelectResultIndex);
}

namespace SteamHelper {
	uintptr_t GetSteamdModule()
	{
		printf("1\n");
		std::string module = ("GameOverlayRenderer64.dll");
		return (uintptr_t)GetModuleHandleA(module.c_str());
	}

	void InsertHook(__int64 addr, __int64 func, __int64* orig)
	{
		printf("2\n");
		static uintptr_t hook_addr;
		if (!hook_addr)
			hook_addr = Scanners::PatternScan(GetSteamdModule(), ("48 ? ? ? ? 57 48 83 EC 30 33 C0"));

		auto hook = ((__int64(__fastcall*)(__int64 addr, __int64 func, __int64* orig, __int64 smthng))(hook_addr));
		hook((__int64)addr, (__int64)func, orig, (__int64)1);
	}

	short GetAsyncKeyState(const int vKey)
	{
		printf("3\n");
		static uintptr_t addrGetAsyncKeyState = NULL;

		if (!addrGetAsyncKeyState)
		{
			addrGetAsyncKeyState = Scanners::PatternScan(GetSteamdModule(),
				("40 53 48 83 EC 20 80 3D ? ? ? ? ? 8B D9"));
		}

		if (!addrGetAsyncKeyState)
			return false;

		using GetAsyncKeyState_t = short(__fastcall*)(int);
		auto fnGetAyncKeyState = (GetAsyncKeyState_t)addrGetAsyncKeyState;

		return fnGetAyncKeyState(vKey);
	}

	short SetCursorPos(int x, int y)
	{
		printf("4\n");
		static uintptr_t addrSetCursorPos = NULL;

		if (!addrSetCursorPos)
		{
			addrSetCursorPos = Scanners::PatternScan(GetSteamdModule(),
				(""));
		}

		if (!addrSetCursorPos)
			return false;

		using SetCursorPos_t = short(__fastcall*)(int, int);
		auto fnSetCursorPos = (SetCursorPos_t)addrSetCursorPos;

		return fnSetCursorPos(x, y);
	}


	bool GetCursorPos(LPPOINT lpPoint)
	{
		printf("5\n");
		static uintptr_t addrGetCursorPos = NULL;

		if (!addrGetCursorPos)
		{
			addrGetCursorPos = Scanners::PatternScan(GetSteamdModule(),
				(""));
		}

		if (!addrGetCursorPos)
			return false;

		using GetCursorPos_t = short(__fastcall*)(LPPOINT);
		auto fnGetCursorPos = (GetCursorPos_t)addrGetCursorPos;

		return fnGetCursorPos(lpPoint);
	}

	HCURSOR SetCursor(HCURSOR hCursor)
	{
		printf("6\n");
		static uintptr_t addrSetCursor = NULL;

		if (!addrSetCursor)
		{
			addrSetCursor = Scanners::PatternScan(GetSteamdModule(),
				(""));
		}

		if (!addrSetCursor)
			return (HCURSOR)false;

		using SetCursor_t = HCURSOR(__fastcall*)(HCURSOR);
		auto fnSetCursor = (SetCursor_t)addrSetCursor;

		return fnSetCursor(hCursor);
	}
}
