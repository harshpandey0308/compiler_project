#compiler

output : main.o tokenizer.o parser.o semantic.o TACcode.o optimizer.o ASMCODE.o preprocessor.o VM.o compiler.o  
	gcc main.o tokenizer.o parser.o semantic.o TACcode.o optimizer.o ASMCODE.o preprocessor.o VM.o compiler.o -o pc

main.o : main.c 
	gcc	-c	main.c

tokenizer.o : tokenizer.c tokenizer.h
	gcc -c tokenizer.c 

parser.o : parser.c	parser.h
	gcc -c parser.c

semantic.o : semantic.c	semantic.h
	gcc -c semantic.c

TACcode.o : TACcode.c	TACcode.h
	gcc -c TACcode.c 

optimizer.o : optimizer.c optimizer.h
	gcc -c optimizer.c 

ASMCODE.o : ASMCODE.c ASMCODE.h
	gcc -c ASMCODE.c 

Preprocessor.o : preprocessor.c	preprocessor.h
	gcc -c preprocessor.c 

VM.o : VM.c VM.h
	gcc -c VM.c

compiler.o : compiler.c compiler.h
	gcc -c compiler.c

clean :
	rm *.o pc