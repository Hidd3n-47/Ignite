#include <cassert>
#include <windows.h>
#include <dbghelp.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")

static const char* kSymbolPath = "E:\\Programming\\Ignite\\Scratch\\Bin\\Dev\\Symbols;";

struct Function
{
    Function(const uint64_t startTime, std::string functionName)
        : startTime(startTime), functionName(std::move(functionName)) {}

    uint64_t startTime;
    uint64_t endTime{};

    std::string functionName;
};

int currentFunctionLevel = -1;
static std::vector<std::vector<Function>> functions;

static bool InitSymbols(HANDLE hProcess) {
    DWORD opts = SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES |
        SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_NO_PROMPTS;
    SymSetOptions(opts);

    if (!SymInitialize(hProcess, kSymbolPath, TRUE)) {
        std::cerr << "SymInitialize failed: " << GetLastError() << "\n";
        return false;
    }

    char buf[4096];
    if (SymGetSearchPath(hProcess, buf, sizeof(buf))) {
        std::cout << "Symbol search path: " << buf << "\n";
    }
    return true;
}

//static void RefreshModules(HANDLE hProcess) {
//    if (WaitForSingleObject(hProcess, 0) == WAIT_TIMEOUT) {
//        if (!SymRefreshModuleList(hProcess)) {
//            DWORD err = GetLastError();
//            if (err != ERROR_SUCCESS) {
//                std::cerr << "SymRefreshModuleList failed: " << err << "\n";
//            }
//        }
//    }
//}

static void LogModuleSymbols(HANDLE hProcess, const char* header) {
    std::cout << "\n--- " << header << " ---\n";
    HMODULE mods[1024];
    DWORD needed = 0;
    if (!EnumProcessModules(hProcess, mods, sizeof(mods), &needed)) {
        std::cerr << "EnumProcessModules failed: " << GetLastError() << "\n";
        return;
    }
    size_t count = needed / sizeof(HMODULE);
    for (size_t i = 0; i < count; ++i) {
        MODULEINFO mi{};
        char path[MAX_PATH]{};
        GetModuleInformation(hProcess, mods[i], &mi, sizeof(mi));
        GetModuleFileNameExA(hProcess, mods[i], path, MAX_PATH);

        IMAGEHLP_MODULE64 m{};
        m.SizeOfStruct = sizeof(m);
        if (SymGetModuleInfo64(hProcess, (DWORD64)mi.lpBaseOfDll, &m)) {
            std::cout << "Module: " << (m.ModuleName ? m.ModuleName : path)
                << "  PDB: " << (m.LoadedPdbName ? m.LoadedPdbName : "")
                << "  SymType: " << m.SymType
                << "  HasSymbols: " << (m.SymType != SymNone ? "Yes" : "No")
                << "  UnmatchedPDB: " << (m.PdbUnmatched ? "Yes" : "No")
                << "\n";
        }
    }
    std::cout << "-----------------------------\n\n";
}

static void PrintFrame(HANDLE hProcess, DWORD64 addr, SYMBOL_INFO* si, std::vector<std::string>& thisFrame) 
{
    DWORD64 dispSym = 0;
    if (SymFromAddr(hProcess, addr, &dispSym, si)) {
        //std::cout << si->Name << " + 0x" << std::hex << dispSym << std::dec;

        thisFrame.emplace_back(si->Name);
    }
    else {
        //std::cout << "[Unknown] @ 0x" << std::hex << addr << std::dec;
    }

    //IMAGEHLP_LINE64 line{};
    //line.SizeOfStruct = sizeof(line);
    //DWORD dispLine = 0;
    //if (SymGetLineFromAddr64(hProcess, addr, &dispLine, &line)) 
    //{
    //    //std::cout << " (" << line.FileName << ":" << line.LineNumber << ")";
    //}
}

static void PrintStack(HANDLE hProcess, HANDLE hThread, const CONTEXT& ctx, DWORD machineType, std::vector<std::string>& thisFrame) 
{
    STACKFRAME64 sf{};
    sf.AddrPC.Mode = AddrModeFlat;
    sf.AddrStack.Mode = AddrModeFlat;
    sf.AddrFrame.Mode = AddrModeFlat;

    sf.AddrPC.Offset = ctx.Rip;
    sf.AddrStack.Offset = ctx.Rsp;
    sf.AddrFrame.Offset = ctx.Rbp;

    constexpr size_t maxName = 1024 * 10;
    constexpr size_t symSize = sizeof(SYMBOL_INFO) + maxName;
    SYMBOL_INFO* si = (SYMBOL_INFO*)malloc(symSize);
    ZeroMemory(si, symSize);
    si->SizeOfStruct = sizeof(SYMBOL_INFO);
    si->MaxNameLen = (ULONG)maxName;

    while (StackWalk64(machineType, hProcess, hThread, &sf, (PVOID)&ctx,
        nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
    {
        DWORD64 addr = sf.AddrPC.Offset;
        if (addr == 0) break;

        PrintFrame(hProcess, addr, si, thisFrame);
    }

    free(si);
}

static bool SampleThread(HANDLE hProcess, DWORD threadId, DWORD machineType) {
    HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, threadId);
    if (!hThread) return false;

    std::vector<std::string> thisFrameFunctions;

    //if (SuspendThread(hThread) == (DWORD)-1) {
    //    CloseHandle(hThread);
    //    return false;
    //}

    CONTEXT ctx{}; 
    memset(&ctx, 0, sizeof(CONTEXT));
    ctx.ContextFlags = CONTEXT_FULL;
    bool ok = false;

    if (GetThreadContext(hThread, &ctx)) {
        PrintStack(hProcess, hThread, ctx, machineType, thisFrameFunctions);
        ok = true;
    }

    ResumeThread(hThread);
    CloseHandle(hThread);

    for (auto it = thisFrameFunctions.rbegin(); it != thisFrameFunctions.rend(); ++it)
    {
        if (*it == "main" || it->starts_with("ignite::"))
        {
            uint64_t startTime = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();

            int foundLevel = currentFunctionLevel;
            bool found = false;
            for (auto fit = functions.rbegin(); fit != functions.rend(); ++fit)
            {
                if (fit->back().functionName == *it)
                {
                    found = true;
                    break;
                }

                --foundLevel;
            }

            if (found)
            {
                if (foundLevel != currentFunctionLevel && functions[currentFunctionLevel].back().endTime == 0)
                {
                    size_t startingPopIndex = functions.size() - 1;
                    while (startingPopIndex != foundLevel)
                    {
                        functions[startingPopIndex--].back().endTime = startTime;
                    }
                    functions[foundLevel].emplace_back(startTime, *it);
                }
            }
            else
            {
                functions.emplace_back();
                functions[++currentFunctionLevel].emplace_back(startTime, *it);
            }

            /*if (functions[level].empty())
            {
                functions[level].emplace_back(startTime, *it); 
            }
            else if (functions[level].back().functionName != *it)
            {
                functions[level].back().endTime = startTime;
                functions[level].emplace_back(startTime, *it);
            }*/

            //++level;
        }
    }

    return ok;
}

static DWORD DetectTargetMachine(HANDLE hProcess) 
{
#if defined(_M_X64)
    typedef BOOL(WINAPI* PFN_IsWow64Process2)(HANDLE, USHORT*, USHORT*);
    PFN_IsWow64Process2 pIsWow64Process2 =
        (PFN_IsWow64Process2)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "IsWow64Process2");
    if (pIsWow64Process2) {
        USHORT processMachine = 0, nativeMachine = 0;
        if (pIsWow64Process2(hProcess, &processMachine, &nativeMachine)) {
            if (processMachine == IMAGE_FILE_MACHINE_I386) {
                return IMAGE_FILE_MACHINE_I386; // WOW64 target
            }
        }
    }
    return IMAGE_FILE_MACHINE_AMD64;
#else
    return IMAGE_FILE_MACHINE_I386;
#endif
}

int main() 
{

    STARTUPINFOA si{ sizeof(si) };
    PROCESS_INFORMATION pi{};

    // Path to your target executable
    const char* exe = "E:/Programming/Ignite/Scratch/Bin/Dev/IgniteGame/IgniteGame.exe";
    char cmdLine[] = "";// "\"E:/Programming/Ignite/Scratch/Bin/Dev/IgniteGame/IgniteGame.exe\"";

    // Launch suspended so we can init symbols first
    if (!CreateProcessA(exe, cmdLine, nullptr, nullptr, FALSE,
        CREATE_NEW_CONSOLE, nullptr, nullptr, &si, &pi)) {
        std::cerr << "CreateProcess failed: " << GetLastError() << "\n";
        return 1;
    }

    // Initialize DbgHelp symbol handler
    if (!InitSymbols(pi.hProcess)) {
        std::cerr << "Symbols not initialized; you may only see addresses.\n";
    }

    // Resume the main thread so the process runs
    ResumeThread(pi.hThread);

    // Detect target architecture (x64 vs WOW64 x86)
    DWORD machineType = DetectTargetMachine(pi.hProcess);
    std::cout << "Machine type: "
        << (machineType == IMAGE_FILE_MACHINE_AMD64 ? "x64" : "x86")
        << "\n";

    // Log initial module symbol status
    LogModuleSymbols(pi.hProcess, "Initial module symbol status");

    // Sampling loop until process exits
    const DWORD intervalMs = 1; // sample every 200 ms
    DWORD tick = 0;
    while (WaitForSingleObject(pi.hProcess, 0) != WAIT_OBJECT_0) {
        //SampleAllThreads(pi.hProcess, machineType);
        SampleThread(pi.hProcess, pi.dwThreadId, machineType);

        // Every ~4 seconds, log module symbol status again
        //if ((tick++ % 20) == 0) {
        //    LogModuleSymbols(pi.hProcess, "Periodic module symbol status");
        //}

        Sleep(intervalMs);
    }

    // Final cleanup
    SymCleanup(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return 0;
}
