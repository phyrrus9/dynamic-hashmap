all: hash.h hash_entry.h hash.cpp hash_entry.cpp hash_helpers.cpp hash_functions.cpp hash_entry.cpp hashmap.h
	g++ main.cpp -o test
debug: hash.h hash_entry.h hash.cpp hash_entry.cpp hash_helpers.cpp hash_functions.cpp hash_entry.cpp hashmap.h
	g++ main.cpp -g -o test
clean:
	rm -rf *.h.gch a.out test
	rm -rf *.cpp~ *.h~ #if gedit added these
