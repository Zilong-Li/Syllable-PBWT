
HTSINC   = /usr/local/include
HTSLIB   = /usr/local/lib
INC      =  -I$(HTSINC) -I$(HTSLIB)
LDFLAGS  =  -L$(HTSLIB) -Wl,-rpath,$(HTSLIB)
LIBS     = -lhts -lz -lm -lbz2 -llzma -lcurl -lpthread

all: main

main: server.cpp client.cpp SyllableQuery.cpp SyllableQuery.h
	g++ -std=c++17 -Wshadow -Wall -o server server.cpp -O2 -Wno-unused-result ${INC} $(LIBS) $(LDFLAGS)
	g++ -std=c++17 -Wshadow -Wall -o client client.cpp -O2 -Wno-unused-result ${INC} $(LIBS) $(LDFLAGS)

clean:
	rm -f server client

.PHONY: all clean
