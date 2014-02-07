CC = x86_64-w64-mingw32-g++ -nostdlib -nostartfiles

ASMS = $(wildcard *.s)
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o) $(ASMS:.s=.o)

dll: $(OBJS)
	$(CC) -o test.dll $(OBJS) -shared \
		-Wl,--enable-auto-import \
		-Wl,--no-whole-archive -lkernel32 -lmsvcrt \
		-Wl,-eDllMain

$(OBJS): $(SRCS) $(ASMS)
	$(CC) $(FLAGS) -c $(SRCS) $(ASMS)

clean:
	rm test.dll *.o
