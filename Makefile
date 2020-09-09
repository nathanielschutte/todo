all:
	gcc todo.c -Wall -Werror -o todo.exe
	cp todo.exe C:\mybin\todo.exe
clean:
	rm *.exe
	rm C:\mybin\todo.exe