hedit: ../../src/*.cpp ../../src/*.hpp
	@mkdir -p ../../bin
	g++ -std=c++14 -Wno-psabi -Wall -DNDEBUG -O2 -o ../../bin/hedit ../../src/*.cpp -lncurses

clean:
	@rm -f ../../bin/hedit
