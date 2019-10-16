
CPPFLAGS=-std=c++17 -stdlib=libc++
.PHONY: release
release: release/wc

release/wc: main.cpp Makefile
	@mkdir release 2>/dev/null || [ 1 ]
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -O3 -o release/wc main.cpp

.PHONY: debug
debug: debug/wc
debug/wc: main.cpp Makefile 
	@mkdir debug 2>/dev/null || [ 1 ]
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -O0 -g3 -o debug/wc main.cpp

.PHONY: clean
clean:
	rm -rf debug release