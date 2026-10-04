CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Isrc/include
ifeq ($(OS),Windows_NT)
LDLIBS = -lws2_32
endif

INC_DIR = src/include
SRC_DIR = src

HEADERS = $(INC_DIR)/des.hpp $(INC_DIR)/crypto_utils.hpp $(INC_DIR)/network.hpp $(INC_DIR)/message_protocol.hpp
TARGETS = sender receiver

.PHONY: all clean

all: $(TARGETS)

sender: $(SRC_DIR)/sender.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDLIBS)

receiver: $(SRC_DIR)/receiver.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDLIBS)

clean:
	rm -f $(TARGETS) *.o