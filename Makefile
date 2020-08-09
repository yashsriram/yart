all: raytracer

raytracer: src/main.cpp include/*
	g++ -Wall -std=c++11 -Iinclude src/main.cpp -o raytracer

clean:
	rm -rf raytracer
