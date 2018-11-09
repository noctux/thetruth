THE_TRUTH_VERSION := $(shell cat version)
JAR_FILE := thetruth-$(THE_TRUTH_VERSION)-all.jar
JAR:=build/libs/$(JAR_FILE)
SO:=native/plugin/xmpp.so

.PHONY: all run $(SO) $(JAR)

all: $(SO) $(JAR)

run: all weechat/plugins/$(JAR_FILE)
	LD_LIBRARY_PATH="${JAVA_HOME}/lib/server" weechat -d ./weechat

native/plugin/java/eu_rationality_thetruth_Weechat.h: src/main/java/eu/rationality/thetruth/Weechat.java $(JAR)
	 $(JAVA_HOME)/bin/javac $< -cp $(JAR) -h ./native/plugin/java

$(SO): native/plugin/java/eu_rationality_thetruth_Weechat.h
	make -C ./native/plugin

$(JAR):
	gradle shadowJar

weechat/plugins/$(JAR_FILE): $(JAR)
	ln -frs $< $@
