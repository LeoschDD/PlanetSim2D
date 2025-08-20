NAME = PlanetSim2D
CXX = g++
CXXFLAGS = -std=c++20 -O2 -Wall
LDFLAGS = -Llib -lraylib -lgdi32 -lwinmm
INCLUDES = -Iinclude -Isrc

SRC = src/**.cpp
OUT = build/$(NAME).exe

default:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(INCLUDES) $(LDFLAGS)
