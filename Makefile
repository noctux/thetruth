
JAR:=target/the-truth-1.0-SNAPSHOT-jar-with-dependencies.jar
SO:=native/plugin/xmpp.so

.PHONY: all $(SO) $(JAR)

all: $(SO) $(JAR)

native/plugin/java/eu_rationality_thetruth_Weechat.h: src/main/java/eu/rationality/thetruth/Weechat.java $(JAR)
	 javac $< -cp $(JAR) -h ./native/plugin/java

$(SO): native/plugin/java/eu_rationality_thetruth_Weechat.h
	make -C ./native/plugin

$(JAR):
	mvn package
