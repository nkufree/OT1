.PHYON: exe

exe:
	yacc main.y 
	g++ y.tab.c *.cpp *.h -o main

test:
	./main < input.txt > output.txt