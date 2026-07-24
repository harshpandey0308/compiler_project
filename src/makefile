
CC =  C:/raylib/w64devkit/bin/gcc.exe
all : pc compiler_gui

RAYLIB_INC = -IC:/raylib/raylib/src	\
				-IC:/raylib/raylib/examples/core	\
				-I.

RAYLIB_LIB = -LC:/raylib/raylib/src	\
				-lraylib	\
				-lopengl32	\
				-lgdi32	\
				-lwinmm

COMPILER_OBJS = \
tokenizer.o	\
parser.o	\
semantic.o	\
TACcode.o	\
optimizer.o	\
ASMCODE.o	\
preprocessor.o	\
VM.o	\
compiler.o	

tokenizer.o : tokenizer.c tokenizer.h
	$(CC) -c tokenizer.c 

parser.o : parser.c	parser.h
	$(CC) -c parser.c

semantic.o : semantic.c	semantic.h
	$(CC) -c semantic.c

TACcode.o : TACcode.c	TACcode.h
	$(CC) -c TACcode.c 

optimizer.o : optimizer.c optimizer.h
	$(CC) -c optimizer.c 

ASMCODE.o : ASMCODE.c ASMCODE.h
	$(CC) -c ASMCODE.c 

preprocessor.o : preprocessor.c	preprocessor.h
	$(CC) -c preprocessor.c 

VM.o : VM.c VM.h
	$(CC) -c VM.c

compiler.o : compiler.c compiler.h
	$(CC) -c compiler.c

main.o : main.c 
	$(CC) -c	main.c

pc:	main.o $(COMPILER_OBJS)
	$(CC) main.o $(COMPILER_OBJS)	-o pc

#output : main.o tokenizer.o parser.o semantic.o TACcode.o optimizer.o ASMCODE.o preprocessor.o VM.o compiler.o  
#	gcc main.o tokenizer.o parser.o semantic.o TACcode.o optimizer.o ASMCODE.o preprocessor.o VM.o compiler.o -o pc

compiler_gui : gui_main.o	$(COMPILER_OBJS)
	$(CC) gui_main.o $(COMPILER_OBJS)	\
		-o compiler_gui	\
		$(RAYLIB_LIB)

gui_main.o : gui/gui_main.c
	$(CC) -c gui/gui_main.c $(RAYLIB_INC)

clean:
	-del /Q *.o pc.exe compiler_gui.exe