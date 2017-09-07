# This is a rudimentary Makefile for rebuilding the distribution.
# It needs ant and a C compiler

CC = gcc
JAVAC = javac
JAVAFLAGS = -O -d classes -encoding utf-8

semgrexP:
	mkdir -p classes
#	$(JAVAC) -classpath CLASSPATH:lib/AppleJavaExtensions.jar $(JAVAFLAGS) src/edu/stanford/nlp/*/*.java src/edu/stanford/nlp/*/*/*.java src/edu/stanford/nlp/*/*/*/*.java
	ant
	cd classes ; jar -cf ../Semgrex-Plus.jar edu ; cd ..

	$(CC) Csrc/ApplyDIFFCommands.c -o ApplyDIFFCommands
	$(CC) Csrc/CreateDIFFCommands.c -o CreateDIFFCommands
	$(CC) Csrc/CreateDIFFCommandsTAG.c -o CreateDIFFCommandsTAG
	$(CC) Csrc/CheckRuleIterference.c -o CheckRuleIterference


clean:
	rm -r classes Semgrex-Plus.jar ApplyDIFF* CreateDIFF* CheckRuleIn*
