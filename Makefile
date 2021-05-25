all: VHCreator

VHCreator: VHCreator.o 
	g++ -ansi -O2 -DNDEBUG -D_GLIBCXX_DEBUG -Wall -Wextra -Werror -Wno-sign-compare -Wshadow -o VHCreator VHCreator.o

VHCreator.o: src/VHCreator.cc
	g++ -ansi -O2 -DNDEBUG -D_GLIBCXX_DEBUG -Wall -Wextra -Werror -Wno-sign-compare -Wshadow -c src/VHCreator.cc

clean:
	rm VHCreator.o

fclean: clean
	rm VHCreator