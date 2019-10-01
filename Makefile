all: raytracer.cpp
	g++ -std=c++11 raytracer.cpp -o raytracer

clean:
	rm -rf raytracer