#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define ADD_ORIGINAL(i, name) originalFunctions[i] = GetProcAddress(dll, #name)

#define PROXY(i, name) \
	__declspec(dllexport) INT_PTR __stdcall name() \
	{ \
		return originalFunctions[i](); \
	}

FARPROC originalFunctions[50] = { 0 };

void loadFunctions(HMODULE dll)
{
	ADD_ORIGINAL(0, WinHttpAddRequestHeaders);
	ADD_ORIGINAL(1, WinHttpAutoProxySvcMain);
	ADD_ORIGINAL(2, WinHttpCheckPlatform);
	ADD_ORIGINAL(3, WinHttpCloseHandle);
	ADD_ORIGINAL(4, WinHttpConnect);
	ADD_ORIGINAL(5, WinHttpConnectionDeleteProxyInfo);
	ADD_ORIGINAL(6, WinHttpConnectionFreeNameList);
	ADD_ORIGINAL(7, WinHttpConnectionFreeProxyInfo);
	ADD_ORIGINAL(8, WinHttpConnectionFreeProxyList);
	ADD_ORIGINAL(9, WinHttpConnectionGetNameList);
	ADD_ORIGINAL(10, WinHttpConnectionGetProxyInfo);
	ADD_ORIGINAL(11, WinHttpConnectionGetProxyList);
	ADD_ORIGINAL(12, WinHttpConnectionSetProxyInfo);
	ADD_ORIGINAL(13, WinHttpCrackUrl);
	ADD_ORIGINAL(14, WinHttpCreateProxyResolver);
	ADD_ORIGINAL(15, WinHttpCreateUrl);
	ADD_ORIGINAL(16, WinHttpDetectAutoProxyConfigUrl);
	ADD_ORIGINAL(17, WinHttpFreeProxyResult);
	ADD_ORIGINAL(18, WinHttpGetDefaultProxyConfiguration);
	ADD_ORIGINAL(19, WinHttpGetIEProxyConfigForCurrentUser);
	ADD_ORIGINAL(20, WinHttpGetProxyForUrl);
	ADD_ORIGINAL(21, WinHttpGetProxyForUrlEx);
	ADD_ORIGINAL(22, WinHttpGetProxyResult);
	ADD_ORIGINAL(23, WinHttpGetTunnelSocket);
	ADD_ORIGINAL(24, WinHttpOpen);
	ADD_ORIGINAL(25, WinHttpOpenRequest);
	ADD_ORIGINAL(26, WinHttpProbeConnectivity);
	ADD_ORIGINAL(27, WinHttpQueryAuthSchemes);
	ADD_ORIGINAL(28, WinHttpQueryDataAvailable);
	ADD_ORIGINAL(29, WinHttpQueryHeaders);
	ADD_ORIGINAL(30, WinHttpQueryOption);
	ADD_ORIGINAL(31, WinHttpReadData);
	ADD_ORIGINAL(32, WinHttpReceiveResponse);
	ADD_ORIGINAL(33, WinHttpResetAutoProxy);
	ADD_ORIGINAL(34, WinHttpSaveProxyCredentials);
	ADD_ORIGINAL(35, WinHttpSendRequest);
	ADD_ORIGINAL(36, WinHttpSetCredentials);
	ADD_ORIGINAL(37, WinHttpSetDefaultProxyConfiguration);
	ADD_ORIGINAL(38, WinHttpSetOption);
	ADD_ORIGINAL(39, WinHttpSetStatusCallback);
	ADD_ORIGINAL(40, WinHttpSetTimeouts);
	ADD_ORIGINAL(41, WinHttpTimeFromSystemTime);
	ADD_ORIGINAL(42, WinHttpTimeToSystemTime);
	ADD_ORIGINAL(43, WinHttpWebSocketClose);
	ADD_ORIGINAL(44, WinHttpWebSocketCompleteUpgrade);
	ADD_ORIGINAL(45, WinHttpWebSocketQueryCloseStatus);
	ADD_ORIGINAL(46, WinHttpWebSocketReceive);
	ADD_ORIGINAL(47, WinHttpWebSocketSend);
	ADD_ORIGINAL(48, WinHttpWebSocketShutdown);
	ADD_ORIGINAL(49, WinHttpWriteData);

}

extern "C" {
	PROXY(0, WinHttpAddRequestHeaders);
	PROXY(1, WinHttpAutoProxySvcMain);
	PROXY(2, WinHttpCheckPlatform);
	PROXY(3, WinHttpCloseHandle);
	PROXY(4, WinHttpConnect);
	PROXY(5, WinHttpConnectionDeleteProxyInfo);
	PROXY(6, WinHttpConnectionFreeNameList);
	PROXY(7, WinHttpConnectionFreeProxyInfo);
	PROXY(8, WinHttpConnectionFreeProxyList);
	PROXY(9, WinHttpConnectionGetNameList);
	PROXY(10, WinHttpConnectionGetProxyInfo);
	PROXY(11, WinHttpConnectionGetProxyList);
	PROXY(12, WinHttpConnectionSetProxyInfo);
	PROXY(13, WinHttpCrackUrl);
	PROXY(14, WinHttpCreateProxyResolver);
	PROXY(15, WinHttpCreateUrl);
	PROXY(16, WinHttpDetectAutoProxyConfigUrl);
	PROXY(17, WinHttpFreeProxyResult);
	PROXY(18, WinHttpGetDefaultProxyConfiguration);
	PROXY(19, WinHttpGetIEProxyConfigForCurrentUser);
	PROXY(20, WinHttpGetProxyForUrl);
	PROXY(21, WinHttpGetProxyForUrlEx);
	PROXY(22, WinHttpGetProxyResult);
	PROXY(23, WinHttpGetTunnelSocket);
	PROXY(24, WinHttpOpen);
	PROXY(25, WinHttpOpenRequest);
	PROXY(26, WinHttpProbeConnectivity);
	PROXY(27, WinHttpQueryAuthSchemes);
	PROXY(28, WinHttpQueryDataAvailable);
	PROXY(29, WinHttpQueryHeaders);
	PROXY(30, WinHttpQueryOption);
	PROXY(31, WinHttpReadData);
	PROXY(32, WinHttpReceiveResponse);
	PROXY(33, WinHttpResetAutoProxy);
	PROXY(34, WinHttpSaveProxyCredentials);
	PROXY(35, WinHttpSendRequest);
	PROXY(36, WinHttpSetCredentials);
	PROXY(37, WinHttpSetDefaultProxyConfiguration);
	PROXY(38, WinHttpSetOption);
	PROXY(39, WinHttpSetStatusCallback);
	PROXY(40, WinHttpSetTimeouts);
	PROXY(41, WinHttpTimeFromSystemTime);
	PROXY(42, WinHttpTimeToSystemTime);
	PROXY(43, WinHttpWebSocketClose);
	PROXY(44, WinHttpWebSocketCompleteUpgrade);
	PROXY(45, WinHttpWebSocketQueryCloseStatus);
	PROXY(46, WinHttpWebSocketReceive);
	PROXY(47, WinHttpWebSocketSend);
	PROXY(48, WinHttpWebSocketShutdown);
	PROXY(49, WinHttpWriteData);
}

HMODULE mHinst = 0, mHinstDLL = 0;
bool hooked = false;

// Loads the original DLL from the default system directory
//	Function originally written by Michael Koch
void load_original_dll()
{
	char buffer[MAX_PATH];

	// Get path to system dir and to winhttp.dll
	GetSystemDirectoryA(buffer, MAX_PATH);

	// Append DLL name
	strcat_s(buffer, "\\winhttp.dll");

	// Try to load the system's winhttp.dll, if pointer empty
	if (!mHinstDLL) {
		mHinstDLL = LoadLibraryA(buffer);
	}

	// Debug
	if (!mHinstDLL) {
		OutputDebugStringA("PROXYDLL: Original winhttp.dll not loaded ERROR ****\r\n");
		ExitProcess(0); // Exit the hard way
	}
}

#define LOG(msg, ...) printf(msg, __VA_ARGS__);

#include <funchook.h>
#include <thread>

#define CHECK_FUNCHOOK_ERR(arg, message) \
	if (arg != FUNCHOOK_ERROR_SUCCESS) printf("%s%s: %s", message, "fn", funchook_error_message(handle))

using byte = uint8_t;

static uint64_t(*The_Second_Copy)(byte* param);
static void(*Decrypt_Fn)(byte* param1, byte* param2, uint64_t size);
static uint64_t(*Second_Inner)(byte* param1, byte* param2, byte* param3);
static void(*Some_Ogg_Function)(byte* param);
static void*(*FPakFile_Ctor)(void *ths, void *LowerLevel, const wchar_t *FileName, bool bIsSigned, bool bLoadIndex);
static void(*FPakFile_Initialize)(void *ths, void *Reader, bool bLoadIndex);
static void(*OnCrashFunc)(void* p1, void* p2, int p3);
static bool(*FPakPlatformFile_Initialize)(void* Inner, const TCHAR* CmdLine);
static void(*GetMasterHash)(void *p1, void *p2, void* outHash);
static void*(*GetPakSignatureFile)(void *ths, TCHAR* InFilename);
static uint64_t(*HashAndGetSize)(void*p1, void*p2, void*p3);
static void(*DoSignatureCheck)(void *ths, byte bWasCanceled, void *Request, int Index);
static bool(*CheckSignature)(void *ths, byte *ChunkInfo);


static byte* Base;

template<typename T>
inline T* offset(void* ptr, size_t offset) {
	return (T*)(reinterpret_cast<byte*>(ptr) + offset);
}

inline byte* offset(byte* ptr, size_t offset) {
	return ptr + offset;
}

struct ThreadData {
	bool doing_unencryption = false;
	bool do_unsigned = false;
	byte* neededHash;
};

static std::unordered_map<std::thread::id, ThreadData> DOING_UNENCRYPTION;


uint64_t Second_Inner_Hook(byte* param1, byte* param2, byte* param3) {
	auto && data = DOING_UNENCRYPTION[std::this_thread::get_id()];
	
	data.doing_unencryption = false;
	auto ret = Second_Inner(param1, param2, param3);
	data.doing_unencryption = false;

	return ret;
}

uint64_t The_Second_Copy_Hook(byte* param) {
	auto ret = The_Second_Copy(param);

	if (*reinterpret_cast<bool*>(param + 0x51) && *reinterpret_cast<bool*>(param + 0x1ba)) {
		printf("Found unencrypted mogg! Setting the correct flags...\n");

		auto &&data = DOING_UNENCRYPTION[std::this_thread::get_id()];
		data.doing_unencryption = true;

		
		auto fn = (void(*)(byte*, uint64_t, uint32_t))(Base + (0x7ff68f31e2c0 - 0x7ff68db60000));
		fn(*(byte **)(param + 0x40), *offset<uint32_t>(*(byte**)offset(param,0x30), 0x4e8) , 0);

		*reinterpret_cast<bool*>(param + 0x1ba) = false;
		return 1;
	}

	return ret;
}

static void Decrypt_Fn_Hook(byte* param1, byte* param2, uint64_t size) {
	auto &&data = DOING_UNENCRYPTION[std::this_thread::get_id()];
	if (data.doing_unencryption) {
		return;
	}

	return Decrypt_Fn(param1, param2, size);
}

void* FPakFile_Ctor_Hook(void *ths, void *LowerLevel, const wchar_t *FileName, bool bIsSigned, bool bLoadIndex) {
	auto &&data = DOING_UNENCRYPTION[std::this_thread::get_id()];
	data.do_unsigned = wcsstr(FileName, L"_P") != nullptr;

	void *ret = FPakFile_Ctor(ths, LowerLevel, FileName, bIsSigned, bLoadIndex);
	printf("FPakFile - %ls (%s | %s)\n", FileName, bIsSigned ? "signed" : "not signed", bLoadIndex ? "load index" : "don't load index");
	data.do_unsigned = false;

	return ret;
}

void *GetPakSignatureFile_Hook(void *ths, TCHAR *InFilename) {
	auto &&data = DOING_UNENCRYPTION[std::this_thread::get_id()];
	printf("Pak Signature for %ls\n", InFilename);
	data.do_unsigned = wcsstr(InFilename, L"_P") != nullptr;
	void* ret = GetPakSignatureFile(ths, InFilename);
	data.do_unsigned = false;

	return ret;
}

uint64_t HashAndGetSize_Hook(void*p1, void**p2, void*p3) {
	auto ret = HashAndGetSize(p1, p2, p3);
	if (ret == 0xffffffff) {
		return ret;
	}

	printf("Size: %zd\n", ret);
	auto &&data = DOING_UNENCRYPTION[std::this_thread::get_id()];
	data.neededHash = reinterpret_cast<byte*>(*p2);

	printf("Copying Bytes: ");
	for (int i = 0; i < ret; i++)
		printf("%02x ", reinterpret_cast<byte*>(*p2)[i]);
	printf("\n");

	return ret;
}

void GetMasterHash_Hook(void *p1, void *p2, void* outHash) {
	GetMasterHash(p1, p2, outHash);
	auto &&data = DOING_UNENCRYPTION[std::this_thread::get_id()];
	if (data.do_unsigned) {
		auto sz = 0x14;

		printf("Injecting Hash!\n");
		printf("Prev Hash: ");
		for (int i = 0; i < sz; i++)
			printf("%02x ", reinterpret_cast<byte*>(outHash)[i]);
		printf("\n");

		memcpy(outHash, data.neededHash, sz);

		printf("New Hash: ");
		for (int i = 0; i < sz; i++)
			printf("%02x ", reinterpret_cast<byte*>(outHash)[i]);
		printf("\n");
	}
}

void DoSignatureCheck_Hook(void *ths, byte bWasCanceled, void *Request, int Index) {
	//Just skip the whole signature check
	auto continueFn = (void(*)(void *ths, byte bWasCanceled, void *Request, int Index))(Base + (0x7ff74d0c7880 - 0x7ff74b930000));
	continueFn(ths, bWasCanceled, Request, Index);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<char> HexToBytes(const std::string& hex) {
	std::vector<char> bytes;

	for (unsigned int i = 0; i < hex.length(); i += 2) {
		std::string byteString = hex.substr(i, 2);
		char byte = (char)strtol(byteString.c_str(), NULL, 16);
		bytes.push_back(byte);
	}

	return bytes;
}



void setup_hook(void **target, void *hook) {
	printf("Preparing to hook function...\n");
	auto handle = funchook_create();
	CHECK_FUNCHOOK_ERR(funchook_prepare(handle, target, hook), "funchook_prepare returned error: ");
	CHECK_FUNCHOOK_ERR(funchook_install(handle, 0), "funchook_install returned error:");
	printf("Sucessfully hooked function!\n");
}

void FUSER_HOOK() {
	Base = (byte*)GetModuleHandleA("Fuser-Win64-Shipping.exe");
	if (Base == 0) {
		printf("ERROR! Base wasn't found!\n");
		return;
	}
	
	The_Second_Copy = (decltype(The_Second_Copy))(Base + (0x7ff68fda5700 - 0x7ff68db60000));
	Decrypt_Fn = (decltype(Decrypt_Fn))(Base + (0x7ff68fda5ad0 - 0x7ff68db60000));
	Second_Inner = (decltype(Second_Inner))(Base + (0x7ff68fda4a10 - 0x7ff68db60000));
	FPakFile_Ctor = (decltype(FPakFile_Ctor))(Base + (0x7ff68f2e7470 - 0x7ff68db60000));
	OnCrashFunc = (decltype(OnCrashFunc))(Base + (0x7ff74c2f6870 - 0x7ff74b930000));
	GetMasterHash = (decltype(GetMasterHash))(Base + (0x7ff74c2abd20 - 0x7ff74b930000));
	GetPakSignatureFile = (decltype(GetPakSignatureFile))(Base + (0x7ff74d0c26b0 - 0x7ff74b930000));
	HashAndGetSize = (decltype(HashAndGetSize))(Base + (0x7ff74d0b0930 - 0x7ff74b930000));
	DoSignatureCheck = (decltype(DoSignatureCheck))(Base + (0x7ff74d0be3f0 - 0x7ff74b930000));

	setup_hook((void**)&The_Second_Copy, The_Second_Copy_Hook);
	setup_hook((void**)&Decrypt_Fn, Decrypt_Fn_Hook);
	setup_hook((void**)&Second_Inner, Second_Inner_Hook);
	setup_hook((void**)&FPakFile_Ctor, FPakFile_Ctor_Hook);
	setup_hook((void**)&GetMasterHash, GetMasterHash_Hook);
	setup_hook((void**)&GetPakSignatureFile, GetPakSignatureFile_Hook);
	setup_hook((void**)&HashAndGetSize, HashAndGetSize_Hook);
	setup_hook((void**)&DoSignatureCheck, DoSignatureCheck_Hook);
}

BOOL WINAPI DllMain(HMODULE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	mHinst = hinstDLL;
	if (fdwReason == DLL_PROCESS_ATTACH) {
		load_original_dll();
		loadFunctions(mHinstDLL);
	}
	else if (fdwReason == DLL_PROCESS_DETACH) {
		FreeLibrary(mHinstDLL);
	}
	
	if (hooked) {
		return (TRUE);
	}
	
	if (fdwReason == DLL_THREAD_ATTACH) {
		hooked = true;
		
//To Enable Console
#if 0
		//Use existing console, or create one if needed
		if (!AttachConsole(-1)) {
			AllocConsole();
			ShowWindow(GetConsoleWindow(), SW_SHOW);
		}
	
		//Redirect output to console
		FILE* fp;
		freopen_s(&fp, "CONOIN$", "r", stdin);
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);
		
#endif

		FUSER_HOOK();
	}

	return (TRUE);
}