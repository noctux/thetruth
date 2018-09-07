WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline \
            -Wuninitialized -Wconversion \
			-Wno-variadic-macros # Disabled due to weechat-plugin.h
          # -Wstrict-prototypes # Disabled due to weechat-plugin.h
INCLUDES := -I/usr/include/weechat \
            -I/usr/lib/jvm/java-10-openjdk/include -I/usr/lib/jvm/java-10-openjdk/include/linux
LINKLIBS := -L/usr/lib/jvm/java-10-openjdk/lib/server -ljvm -pthread

CFLAGS   := -g -std=c11 -fPIC -pthread -D_XOPEN_SOURCE=500 $(WARNINGS) $(INCLUDES)
LDFLAGS  := -shared -fPIC

SRCFILES := $(wildcard *.c)
OBJFILES := $(patsubst %.c,%.o,$(SRCFILES))
DEPFILES := $(patsubst %.c,%.d,$(SRCFILES))

.PHONY: clean all

TARGET := xmpp.so
all: $(TARGET) java/Main.class

-include $(DEPFILES)

$(TARGET): $(OBJFILES)
	$(CC) $(LDFLAGS) $^ $(LINKLIBS) -o $@

%.o: %.c Makefile
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

%.class: %.java Makefile
	javac $< -h java

%.h: %.java Makefile
	javac $< -h java
