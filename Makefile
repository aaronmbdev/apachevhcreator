all: VHCreator

VHCreator: VHCreator.o 
	gcc -o VHCreator VHCreator.o

VHCreator.o: src/VHCreator.c
	gcc -c src/VHCreator.c

clean:
	rm VHCreator.o

fclean: clean
	rm VHCreator