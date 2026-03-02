#include <windows.h>
#include <mmsystem.h>
#include <stdint.h>

typedef struct PatchByte { uint32_t offset; uint8_t value; } PatchByte;

static HMODULE g_real = NULL;

static const PatchByte kPatches[] = {
    {0x00083AD5, 0xEB}, {0x00083B0A, 0xEB}, {0x00083B46, 0xEB}, {0x00083B7B, 0xEB},
    {0x00083BB7, 0xEB}, {0x00083BEC, 0xEB}, {0x00083C65, 0x83}, {0x00083C66, 0xC4},
    {0x00083C67, 0x04}, {0x00083C68, 0x90}, {0x00083C69, 0x90}, {0x00083CAC, 0xEB},
    {0x000850D8, 0xB2}, {0x000850D9, 0x34}, {0x000850DA, 0x90}, {0x000850DB, 0x90},
};

static void load_real(void) {
    if (!g_real) g_real = LoadLibraryA("C:\\Windows\\SysWOW64\\winmm.dll");
}

static FARPROC sym(const char* n) {
    load_real();
    return g_real ? GetProcAddress(g_real, n) : NULL;
}

static void apply_patch(void) {
    uint8_t* base = (uint8_t*)GetModuleHandleA(NULL);
    if (!base) return;
    DWORD oldp = 0;
    uint8_t* region = base + 0x00082000;
    SIZE_T size = 0x00004000;
    if (!VirtualProtect(region, size, PAGE_EXECUTE_READWRITE, &oldp)) return;
    for (size_t i = 0; i < sizeof(kPatches)/sizeof(kPatches[0]); ++i) base[kPatches[i].offset] = kPatches[i].value;
    FlushInstructionCache(GetCurrentProcess(), region, size);
    DWORD tmp = 0; VirtualProtect(region, size, oldp, &tmp);
}

#define RESOLVE(fn) static FARPROC p_##fn = NULL; if (!p_##fn) p_##fn = sym(#fn); if (!p_##fn) return 0

__declspec(dllexport) DWORD WINAPI proxy_timeGetTime(void){ RESOLVE(timeGetTime); return ((DWORD (WINAPI*)(void))p_timeGetTime)(); }
__declspec(dllexport) FOURCC WINAPI proxy_mmioStringToFOURCCA(LPCSTR s, UINT f){ RESOLVE(mmioStringToFOURCCA); return ((FOURCC (WINAPI*)(LPCSTR,UINT))p_mmioStringToFOURCCA)(s,f); }
__declspec(dllexport) MMRESULT WINAPI proxy_timeBeginPeriod(UINT u){ RESOLVE(timeBeginPeriod); return ((MMRESULT (WINAPI*)(UINT))p_timeBeginPeriod)(u); }
__declspec(dllexport) MMRESULT WINAPI proxy_timeEndPeriod(UINT u){ RESOLVE(timeEndPeriod); return ((MMRESULT (WINAPI*)(UINT))p_timeEndPeriod)(u); }
__declspec(dllexport) HMMIO WINAPI proxy_mmioOpenA(LPSTR s, LPMMIOINFO i, DWORD f){ RESOLVE(mmioOpenA); return ((HMMIO (WINAPI*)(LPSTR,LPMMIOINFO,DWORD))p_mmioOpenA)(s,i,f); }
__declspec(dllexport) MMRESULT WINAPI proxy_mmioClose(HMMIO h, UINT f){ RESOLVE(mmioClose); return ((MMRESULT (WINAPI*)(HMMIO,UINT))p_mmioClose)(h,f); }
__declspec(dllexport) MMRESULT WINAPI proxy_mmioDescend(HMMIO h, LPMMCKINFO c, const MMCKINFO* p, UINT f){ RESOLVE(mmioDescend); return ((MMRESULT (WINAPI*)(HMMIO,LPMMCKINFO,const MMCKINFO*,UINT))p_mmioDescend)(h,c,p,f); }
__declspec(dllexport) MMRESULT WINAPI proxy_mmioAscend(HMMIO h, LPMMCKINFO c, UINT f){ RESOLVE(mmioAscend); return ((MMRESULT (WINAPI*)(HMMIO,LPMMCKINFO,UINT))p_mmioAscend)(h,c,f); }
__declspec(dllexport) LONG WINAPI proxy_mmioRead(HMMIO h, HPSTR p, LONG c){ RESOLVE(mmioRead); return ((LONG (WINAPI*)(HMMIO,HPSTR,LONG))p_mmioRead)(h,p,c); }
__declspec(dllexport) LONG WINAPI proxy_mmioWrite(HMMIO h, const char* p, LONG c){ RESOLVE(mmioWrite); return ((LONG (WINAPI*)(HMMIO,const char*,LONG))p_mmioWrite)(h,p,c); }
__declspec(dllexport) MMRESULT WINAPI proxy_mmioCreateChunk(HMMIO h, LPMMCKINFO c, UINT f){ RESOLVE(mmioCreateChunk); return ((MMRESULT (WINAPI*)(HMMIO,LPMMCKINFO,UINT))p_mmioCreateChunk)(h,c,f); }
__declspec(dllexport) LONG WINAPI proxy_mmioSeek(HMMIO h, LONG l, int o){ RESOLVE(mmioSeek); return ((LONG (WINAPI*)(HMMIO,LONG,int))p_mmioSeek)(h,l,o); }

BOOL WINAPI DllMain(HINSTANCE h, DWORD r, LPVOID p){ (void)p; if(r==DLL_PROCESS_ATTACH){ DisableThreadLibraryCalls(h); load_real(); apply_patch(); } return TRUE; }


