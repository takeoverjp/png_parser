CXXFLAGS=-Og -g -W -Wall -Werror -std=c++11
LDFLAGS=-lpng

png_parser: png_parser.cpp
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

test: png_parser
	./$< testdata/lena.png

clean:
	rm -f png_parser *~ *.o

.PHONY: clean test
