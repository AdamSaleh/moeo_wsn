all : evolve.cpp
	/usr/gcc_4_7/bin/g++-4.7 -std=c++11 -DDEPRECATED_MESSAGES -fopenmp -Wunknown-pragmas -O0 -g -I/usr/include/python2.7 -I/home/asaleh/Diplomka/ParadisEO-2.0/smp/src -I/usr/local/local/include/paradiseo -I/usr/local/local/include/paradiseo/eo -I/usr/local/local/include/paradiseo/mo -I/usr/local/local/include/paradiseo/moeo -oevolve.o -c evolve.cpp
	ar cr evolve.a evolve.o
	/usr/gcc_4_7/bin/g++-4.7 -fopenmp -Wunknown-pragmas  -g  evolve.o -o evolve -rdynamic -L"/usr/gcc_4_7/lib" -L"/usr/gcc_4_7/lib64" "/home/asaleh/Diplomka/ParadisEO-2.0/build/eo/lib/libeo.a" "/home/asaleh/Diplomka/ParadisEO-2.0/build/eo/lib/libga.a" "/home/asaleh/Diplomka/ParadisEO-2.0/build/eo/lib/libeoutils.a" "/usr/local/local/lib64/libsmp.a" "/home/asaleh/Diplomka/ParadisEO-2.0/build/moeo/lib/libmoeo.a" -lpython2.7 
	rm evolve.o
	rm evolve.a

clean :
	rm evolve
