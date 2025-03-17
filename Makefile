TARGET = huffman_compressor

CXX = g++

# CXXFLAGS = -Wall -Wextra -std=c++17
CXXFLAGS = -std=c++17

SRCS = main.cpp huffman.cpp file_utils.cpp

OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	del /f $(OBJS) $(TARGET)

rebuild: clean $(TARGET)
