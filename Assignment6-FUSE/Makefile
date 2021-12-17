FUSE_FLAGS := $(shell pkg-config fuse --cflags --libs)

all:
	g++ -std=c++17 -Wall -Wextra -o fuse fuse.cpp $(FUSE_FLAGS)
