CFLAGS		+= -std=gnu11 -fansi-escape-codes -fcaret-diagnostics -fcolor-diagnostics -fmessage-length=0 -fPIE -fstack-protector -fno-common -W -Wall -Werror -Wcast-qual -Wcomma -Wconditional-uninitialized -Wconversion -Wconversion-null -Wdeprecated -Wduplicate-method-arg -Wduplicate-method-match -Winfinite-recursion -Wmissing-declarations -Wnullable-to-nonnull-conversion -Wpointer-arith -Wstrict-prototypes -Wthread-safety -Wundef -Wunreachable-code-aggressive -Wunused-local-typedef -Wunused-result -Wvla -Wwrite-strings -Wno-gnu-zero-line-directive -Wno-gnu-zero-variadic-macro-arguments -Wno-unknown-pragmas -Wno-unused-label -Wno-unused-parameter
CPPFLAGS	+= $(shell pkg-config --cflags-only-I liblz4)
LDFLAGS		+= $(shell pkg-config --libs-only-L liblz4) -fPIE -Xlinker -pie
LDLIBS		:= $(shell pkg-config --libs-only-l liblz4) -lcompression

SOURCES		:= $(wildcard *.c)
OBJECTS		:= $(SOURCES:%.c=%.o)
TARGET		:= mozlz4

.PHONY:		all debug clean

all:		CPPFLAGS += -DNDEBUG -D_FORTIFY_SOURCE=2
all:		CFLAGS += -Os
all:		$(TARGET)

debug:		CFLAGS += -fsanitize=address,undefined -g -Og
debug:		LDFLAGS += -fsanitize=address,undefined
debug:		$(TARGET)

$(TARGET):	$(OBJECTS)

clean:
	$(RM) $(TARGET) $(OBJECTS)
