SERVERSRCS = $(wildcard ./Sources/server.cpp ./Sources/cert.cpp ./Sources/common.cpp)
SERVEROBJS = $(SERVERSRCS:.c = .o)

CLIENTSRCS = $(wildcard ./Sources/client.cpp ./Sources/clientcert.cpp ./Sources/common.cpp)
CLIENTOBJS = $(CLIENTSRCS:.c = .o)

CC = g++
INCLUDES = -I $(PWD)/Headers
CCFLAGS = -std=c++11 -g -Wall -O0 
OUTPUT = server client

all : server client

server:$(SERVEROBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(CCFLAGS) 

client:$(CLIENTOBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(CCFLAGS)

$.o : .c
	$(CC) -o $< $(CCFLAGS)

clean:
	rm -rf server.dSYM* server client.dSYM client

.PHONY : clean