SERVERSRCS = $(wildcard ./Sources/server.cpp ./Sources/cert.cpp)
SERVEROBJS = $(SERVERSRCS:.c = .o)

CLIENTSRCS = $(wildcard ./Sources/client.cpp ./Sources/clientcert.cpp)
CLIENTOBJS = $(CLIENTSRCS:.c = .o)


WORKDIR = /pca
CC = g++
INCLUDES = -I $(WORKDIR)/Headers
CCFLAGS = -std=c++11 -pthread -g -Wall -O0 
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
