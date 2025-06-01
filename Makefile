CXX = g++
CXXFLAGS = -std=c++11 `wx-config --cxxflags`
LIBS = `wx-config --libs`
TARGET = simulator
SOURCE = main.cpp

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE) $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: clean