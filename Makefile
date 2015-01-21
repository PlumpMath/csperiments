CFLAGS=-Wall -g

check-syntax:
	clang -o /dev/null $(CFLAGS) -S ${CHK_SOURCES}
