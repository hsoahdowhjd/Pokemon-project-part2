a.out: main.o pokemon.o move.o map.o player.o item.o
	g++ main.o pokemon.o move.o map.o player.o item.o -lncurses 
main.o: main.cpp
	g++ -Wall -c main.cpp -lncurses
pokemon.o: pokemon.cpp
	g++ -Wall -c pokemon.cpp
move.o: move.cpp
	g++ -Wall -c move.cpp
map.o: map.cpp
	g++ -Wall -c map.cpp
player.o: player.cpp
	g++ -Wall -c player.cpp
item.o: item.cpp
	g++ -Wall -c item.cpp
clean:
	rm -f a.out *.o core
