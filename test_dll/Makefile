CC = x86_64-w64-mingw32-g++ -g -nostdlib -nostartfiles


SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

dll: $(OBJS)
	$(CC) -o test.dll $(OBJS) -shared \
		-Wl,--enable-auto-import \
		-Wl,--no-whole-archive -luser32 -lkernel32 \
		-Wl,-eDllMain

$(OBJS): $(SRCS)
	$(CC) $(FLAGS) -c $(SRCS)

clean:
	rm test.dll *.o
