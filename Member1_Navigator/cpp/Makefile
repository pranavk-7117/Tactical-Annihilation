CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

SRCS = main.cpp GridMap3D.cpp AStarPathfinder.cpp MapSerializer.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = navigator_engine

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) navigator_session.log

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
