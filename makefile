all: run_file

run_file:  toy_printf.o main[1].o
	gcc -g -Wall -o run_file toy_printf.o main[1].o

toy_printf.o: toy_printf.c
	gcc -g -Wall -c -o toy_printf.o toy_printf.c
	
main[1].o: main[1].c
	gcc -g -Wall -c -o main[1].o main[1].c

clean:
	rm -f *.o
 
