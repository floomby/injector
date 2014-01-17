#include <cstdio>

#include "tiblib.h"

void *getApproxStackTop()
{
    void *top;
    __asm__ __volatile__(
            "movq %%rsp, %0"
            : "=r" (top)
            :
            :
    );
    return top;
}

void main2()
{
    printf("the current process id is %d\n", getProcessID());
    printf("the current thread id is %d\n\n", getThreadID());
    fflush(stdout);
    

    getchar();

    return;
}

int main(int argc, char **argv)
{
    // main2();
    
    // puts("the following is a list of loaded modules");
    // printModList();
    // fflush(stdout);
    
    // return 0;
    
    
    puts("waiting: any key to continue (you can attach a debugger)");
    fflush(stdout);
    getchar();

    printf("TIB is at %p\n", getTib());
    printf("the PEB is at %p\n\n", getPeb());

    printf("the stack base is %p\n", getStackBase());
    printf("the stack top is approxamatly at %p\n", getApproxStackTop());
    printf("the stack limit is %p\n", getStackLimit());
    
    if(beingDebugged())
        puts("the process is being debugged\n");
    else
        puts("the process is not being debugged\n");
    
    printf("the current process id is %d\n", getProcessID());
    printf("the current thread id is %d\n\n", getThreadID());
    
    printf("loader data at %p\n", getLdr());
    printf("module list at %p\n\n", getModList());
    
    puts("the following is a list of loaded modules");
    printModList();
    puts("");
    
    void *kernel32_base = getModBase(L"C:\\Windows\\system32\\KERNEL32.DLL");
    printf("kernel32.dll is based at %p\n", kernel32_base);
    void (*sleep_fp)(int) = (void (*)(int))((char *)kernel32_base + 0x1010);
    printf("checking if sleep works");
    fflush(stdout);
    for(int i = 0; i < 4; i++){
        sleep_fp(400);
        putchar('.');
        fflush(stdout);
    }
    sleep_fp(400);
    puts("\nsleep appears to work");
    
    return 0;
}