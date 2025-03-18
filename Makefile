TARGET1 = huffman_compressor

CXX = g++

# Flags
CXXFLAGS = -m64 -std=c++17 -fexceptions
LDFLAGS = -lstdc++

#verificar o SO
ifeq ($(OS), Windows_NT)
TARGET = $(TARGET1).exe
else
TARGET = $(TARGET1).out
endif

SRCS = main.cpp huffman.cpp file_utils.cpp

# OBJS = $(SRCS:.cpp=.o)
OBJS = main.o huffman.o file_utils.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	del /f $(OBJS) $(TARGET)

rebuild: clean $(TARGET)
