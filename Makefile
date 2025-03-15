CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread
LDFLAGS = -lmysqlclient

SRC = main.cpp WebServer.cpp ThreadPool.cpp HttpParser.cpp MySQLConnPool.cpp StudentDB.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = webserver

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)
