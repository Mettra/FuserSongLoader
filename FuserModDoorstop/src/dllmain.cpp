#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Psapi.h>

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
size_t ModuleSize = 0;

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

size_t The_Second_Copy_UnencryptFnRVA;
uint64_t The_Second_Copy_Hook(byte* param) {
	auto ret = The_Second_Copy(param);

	if (*reinterpret_cast<bool*>(param + 0x51) && *reinterpret_cast<bool*>(param + 0x1ba)) {
		printf("Found unencrypted mogg! Setting the correct flags...\n");

		auto &&data = DOING_UNENCRYPTION[std::this_thread::get_id()];
		data.doing_unencryption = true;

		
		auto fn = (void(*)(byte*, uint64_t, uint32_t))(Base + The_Second_Copy_UnencryptFnRVA);
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

size_t DoSignatureCheck_ContinueFnRVA;
void DoSignatureCheck_Hook(void *ths, byte bWasCanceled, void *Request, int Index) {
	//Just skip the whole signature check
	auto continueFn = (void(*)(void *ths, byte bWasCanceled, void *Request, int Index))(Base + DoSignatureCheck_ContinueFnRVA);
	continueFn(ths, bWasCanceled, Request, Index);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<byte> HexToBytes(const std::string& hex) {
	std::vector<byte> bytes;

	for (unsigned int i = 0; i < hex.length(); i += 2) {
		std::string byteString = hex.substr(i, 2);
		byte b = (byte)strtol(byteString.c_str(), NULL, 16);
		bytes.push_back(b);
	}

	return bytes;
}

std::vector<size_t> found_rva;

byte* find_function(const char *name, const char *byteSequence) {
	printf("Searching for %s.\n", name);
	auto data = HexToBytes(byteSequence);

	printf("	Byte Sequence: ");
	for (int i = 0; i < data.size(); i++)
		printf("%02x ", reinterpret_cast<byte*>(data.data())[i]);
	printf("\n");

	for (size_t i = 0; i < ModuleSize; ++i) {
		bool done = true;
		for (size_t j = 0; j < data.size(); ++j) {
			if ((*(Base + i + j) != *(data.data() + j)) && byteSequence[j * 2] != '?') {
				done = false;
				break;
			}
		}

		if (done) {
			printf("	Found! RVA: 0x%x\n", (unsigned int)i);
			found_rva.emplace_back(i);
			return Base + i;
		}
	}

	printf("	No function found with that pattern!\n");
	found_rva.emplace_back(0);
	return nullptr;
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
	
// Change to 0 to re-lookup all of these functions.
#if 1
	The_Second_Copy = (decltype(The_Second_Copy))(Base + 0x2253aa0);
	Decrypt_Fn = (decltype(Decrypt_Fn))(Base + 0x2253e70);
	Second_Inner = (decltype(Second_Inner))(Base + 0x2252db0);
	FPakFile_Ctor = (decltype(FPakFile_Ctor))(Base + 0x17935c0);
	OnCrashFunc = (decltype(OnCrashFunc))(Base + 0x9d2540);
	GetMasterHash = (decltype(GetMasterHash))(Base + 0x9879f0);
	GetPakSignatureFile = (decltype(GetPakSignatureFile))(Base + 0x179e800);
	HashAndGetSize = (decltype(HashAndGetSize))(Base + 0x178ca80);
	DoSignatureCheck = (decltype(DoSignatureCheck))(Base + 0x179a540);


	DoSignatureCheck_ContinueFnRVA = 0x17a39d0;
	The_Second_Copy_UnencryptFnRVA = 0x17ca410;

#else
	MODULEINFO moduleInfo;
	GetModuleInformation(GetCurrentProcess(), (HMODULE)Base, &moduleInfo, sizeof(moduleInfo));
	ModuleSize = moduleInfo.SizeOfImage;

	printf("Functions are unknown. Searching %zd bytes in process...\n\n", ModuleSize);

	find_function("The_Second_Copy", "405556488d6c24b14881ecd8000000488bf1488b89c00100004885c9740de85dc61300ff86c8010000eb0a");
	find_function("Decrypt_Fn", "405641564157b820800000e880235c00482be0488b4130498bf04c8bfa4c8bf1837824ff0f84040100004889bc2450800000");
	find_function("Second_Inner", "48895c2408574883ec204c894140488bd966c741480000498bf848895130c6421801c6413800488b49304883c128");
	find_function("FPakFile_Ctor", "48895c2410555657415641574883ec304533ff410fb6e94c8939498bf04c8979084c8bf2488bd94d85c0743f66453938");
	find_function("OnCrashFunc", "48895c24185556574154415541564157488dac2450faffff4881ecb0060000488b05????????4833c4488985a0050000");
	find_function("GetMasterHash", "4c8bdc534881ec00010000488b05????????4833c448898424f0000000498d4398498bd8498943d84c8bc233c0c744242001234567");
	find_function("GetPakSignatureFile", "405556574154488dac2478ffffff4881ec88010000488b05????????4833c4488945784c8be1488bf2488d0d");
	find_function("HashAndGetSize", "48895c24084889742410574883ec30498bd8488bfa488bf1e8530000004885c0742d0f10064c8b10488d5424204c8bcb4c8bc7488bc80f29442420");
	find_function("DoSignatureCheck", "4055535657415541564157488d6c24f04881ec10010000488b05????????4833c44889450033c04963f1488d590848894c2438");

	find_function("DoSignatureCheck_ContinueFnRVA", "48895c240848896c2410488974241857415641574883ec20488bf14963f94883c108498be8440fb6f2");
	find_function("The_Second_Copy_UnencryptFnRVA", "488b0148ff6068cccccccccccccccccc");

	printf("RVA list:\n");
	for (auto &&rva : found_rva) {
		printf("0x%x\n", (unsigned int)rva);
	}
	found_rva.clear();

	printf("Searches done! Copy the addresses and apply them to the cpp file!\n");
	system("pause");
	_Exit(0);
#endif

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
#if 1
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