CC = g++
CFLAGS = -std=c++11 -O3

generator: generator.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f generator
	rm -f created_instances/*

create_instances: generator
	sh create_instances.sh
