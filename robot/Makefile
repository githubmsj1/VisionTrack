CC=g++
LIB=-ltld -lferNN -lLKTracker  -ltld_utils serial.o `pkg-config --libs opencv`
LIBPATH=-L./
HEAD=-I../include `pkg-config --cflags opencv`
OBJ=run
$(OBJ):intelisu.o serial.o
	$(CC) -o run intelisu.o $(LIBPATH) $(LIB)
intelisu.o:
	$(CC) -c intelisu.cpp $(HEAD)

libferNN.a:ferNN.o
	ar crv libferNN.a ferNN.o
ferNN.o:
	$(CC) -c FerNNClassifier.cpp -o ferNN.o $(HEAD)

libLKTracker.a:LKTracker.o
	ar crv libLKTracker.a LKTracker.o
LKTracker.o:
	$(CC) -c LKTracker.cpp -o LKTracker.o $(HEAD)

libtld_utils.a:tld_utils.o
	ar crv libtld_utils.a tld_utils.o
tld_utils.o:
	$(CC) -c tld_utils.cpp -o tld_utils.o $(HEAD)

libtld.a:tld.o
	ar crv libtld.a tld.o
tld.o:
	$(CC) -c TLD.cpp -o tld.o $(HEAD)
serial.o:
	$(CC) -c serial.cpp -o serial.o $(HEAD)


clean:
	rm run *.o
