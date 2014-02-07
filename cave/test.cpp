#include <windows.h>

__attribute__ ((__optimize__("omit-frame-pointer"))) void loadDll()
{
    __asm__ __volatile__(
        "movq %%rax, -0x08(%%esp);"
        "movq $0xDEADBEEFDEADBEEF, %%rax;"
        "pushq %%rax;"
        "sub $0x08, %%esp;"
        "pushf;"
        "pushq %%rcx;"
        "pushq %%rdx;"
        "pushq %%r8;"
        "pushq %%r9;"
        "pushq %%r10;"
        "pushq %%r11;"
        "movq $0xDEADBEEFDEADBEEF, %%rcx;"
        "movq $0xDEADBEEFDEADBEEF, %%rax;"
        "call *%%rax;"
        "popq %%r11;"
        "popq %%r10;"
        "popq %%r9;"
        "popq %%r8;"
        "popq %%rdx;"
        "popq %%rcx;"
        "popf;"
        "popq %%rax;"
        :
        :
        :
    );
}

void patch(uint8_t *buf, uint64_t ret, uint64_t arg, uint64_t addr)
{
    *(uint64_t *)(buf + 8) = ret;
    *(uint64_t *)(buf + 33) = arg;
    *(uint64_t *)(buf + 43) = addr;
}

#define THREAD_PERMS    (THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME)
#define PROC_PERMS      (PROCESS_VM_WRITE | PROCESS_VM_OPERATION)

void inject(DWORD pid, DWORD tid, const char *name, void *buf, size_t len)
{
    void *load_addr = (void *)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

    HANDLE hProc = OpenProcess(PROC_PERMS, false, pid);
    
    void *dll_str = VirtualAllocEx(hProc, NULL, strlen(name) + 1, MEM_COMMIT, PAGE_READWRITE);
    void *stub    = VirtualAllocEx(hProc, NULL, len, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    WriteProcessMemory(hProc, dll_str, name, strlen(name) + 1, NULL);
    
    CONTEXT ctx;
    
    HANDLE hThread = OpenThread(THREAD_PERMS, false, tid);
    SuspendThread(hThread);
    
    ctx.ContextFlags = CONTEXT_CONTROL;
    GetThreadContext(hThread, &ctx);
    uint64_t rip = ctx.Rip;
    ctx.Rip = (uint64_t)stub;
    ctx.ContextFlags = CONTEXT_CONTROL;
    
    patch((uint8_t *)buf, rip, (uint64_t)dll_str, (uint64_t)load_addr);
    
    WriteProcessMemory(hProc, stub, buf, len, NULL);
    SetThreadContext(hThread, &ctx);
    
    ResumeThread(hThread);
    
    Sleep(3000);
    
    VirtualFreeEx(hProc, dll_str, strlen(name) + 1, MEM_DECOMMIT);
    VirtualFreeEx(hProc, stub, len, MEM_DECOMMIT);
    
    CloseHandle(hProc);
    CloseHandle(hThread);
}

#include <TlHelp32.h>

DWORD getThreadFromPid(DWORD pid)
{
    void * hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if(hThreadSnap == INVALID_HANDLE_VALUE){
        fprintf(stderr, "unable to create snapshot (check privs)\n");
        exit(EXIT_FAILURE);
    }
    
    THREADENTRY32 tEntry;
    tEntry.dwSize = sizeof(THREADENTRY32);
    DWORD ret = 0;
    
    for(BOOL success = Thread32First(hThreadSnap, &tEntry);
        !ret && success && GetLastError() != ERROR_NO_MORE_FILES;
        success = Thread32Next(hThreadSnap, &tEntry)){
            if(tEntry.th32OwnerProcessID == pid){
                ret = tEntry.th32ThreadID;
            }
    }
    
    CloseHandle(hThreadSnap);
    
    return ret;
}

// TODO: we need to pick a good pid to inject into

extern "C" BOOL APIENTRY __declspec(dllexport) DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{   
    switch (fdwReason){
        case DLL_PROCESS_ATTACH:
            break;
        case DLL_PROCESS_DETACH:
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}

/*
int main(int argc, char *argv[])
{
    if(argc != 3){
        fprintf(stderr, "usage: %s <pid> <dllname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    uint8_t buf[66];
    memcpy(buf, (void *)&loadDll, 66);
    
    inject(atoi(argv[1]), getThreadFromPid(atoi(argv[1])), argv[2], buf, 66);
    
    return 0;
}
*/
