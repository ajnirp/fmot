all: fmot

fmot: fmot.o essentials.o fileHandling.o fileIndexing.o luaSearching.o matchWithLua.o renaming.o shiftFiles.o
		g++ fmot.o essentials.o fileHandling.o fileIndexing.o luaSearching.o matchWithLua.o renaming.o shiftFiles.o -o fmot -std=c++0x -lncurses -llua

fmot.o: fmot.cpp
		g++ -c fmot.cpp -std=c++0x -lncurses -llua

essentials.o: essentials.cpp
		g++ -c essentials.cpp -std=c++0x

fileHandling.o: fileHandling.cpp
		g++ -c fileHandling.cpp -std=c++0x

fileIndexing.o: fileIndexing.cpp
		g++ -c fileIndexing.cpp -std=c++0x

luaSearching.o: luaSearching.cpp
		g++ -c luaSearching.cpp -std=c++0x -llua

matchWithLua.o: matchWithLua.cpp
		g++ -c matchWithLua.cpp -std=c++0x -llua

renaming.o: renaming.cpp
		g++ -c renaming.cpp -std=c++0x

shiftFiles.o: shiftFiles.cpp
		g++ -c shiftFiles.cpp -std=c++0x

clean:
		rm -rf *.o fmot
