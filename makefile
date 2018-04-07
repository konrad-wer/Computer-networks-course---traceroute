
traceroute : makefile main.o send.o receive.o 
	gcc -o traceroute main.o send.o receive.o -std=c99 -Wall -Wextra
	
main.o : makefile main.c send.o receive.o 
	gcc -c -o main.o main.c -std=c99 -Wall -Wextra

send.o : makefile send.h send.c
	gcc -c -o send.o send.c -std=c99 -Wall -Wextra

receive.o : makefile receive.h  receive.c
	gcc -c -o receive.o receive.c -std=c99 -Wall -Wextra

clean :
	rm *.o

distclean :
	rm *.o
	rm traceroute
