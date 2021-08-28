#include "utils.h"

void** draw_hud_hook;

namespace render
{
	void draw_hud(void* hud)
	{
		auto UCanvas = *(uint64_t*)((uint64_t*)hud + utils::UCanvas::Canvas);
	}
	
	bool initalize_hud()
	{
		draw_hud(draw_hud_hook);
		//return 1;
	}
}

void initalize_hooks()
{
	MH_Initialize();

	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
}

bool __stdcall DllMain(void* hModule, unsigned long ul_reason_for_call, void* lpReserved)
{
	if (ul_reason_for_call == 1)
	{
		initalize_hooks();
		return TRUE;
	}
	return 1;
}