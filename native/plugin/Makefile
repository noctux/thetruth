WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline \
            -Wuninitialized -Wconversion \
			-Wno-variadic-macros # Disabled due to weechat-plugin.h
          # -Wstrict-prototypes # Disabled due to weechat-plugin.h
INCLUDES := -I/usr/include/weechat \
            -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
LINKLIBS := -L$(JAVA_HOME)/lib/server -ljvm -pthread

THE_TRUTH_VERSION=$(shell cat ../../version)
CFLAGS   := -g -std=c11 -fPIC -pthread -D_XOPEN_SOURCE=500 -DTHE_TRUTH_VERSION=$(THE_TRUTH_VERSION) $(WARNINGS) $(INCLUDES)
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
