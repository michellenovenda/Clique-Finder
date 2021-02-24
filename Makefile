all:main.cpp
	g++ -lm -std=c++11 -c *.cpp
	g++ -lm -std=c++11 -c *.cpp
	g++ -lm -std=c++11 -o clique_find *.o