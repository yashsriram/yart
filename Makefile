all: ray_caster.cpp
	g++ -std=c++11 ray_caster.cpp -o ray_caster

clean:
	rm -rf ray_caster