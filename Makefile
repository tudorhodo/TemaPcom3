all: client

client: client.o requests.o helpers.o buffer.o
	g++ -o client client.o requests.o helpers.o buffer.o -Wall

client.o:client.cpp
	g++ -c client.cpp

requests.o:requests.cpp
	g++ -c requests.cpp

helpers.o:helpers.cpp
	g++ -c helpers.cpp

buffer.o:buffer.cpp
	g++ -c buffer.cpp

run: client
	./client

clean:
	rm -f *.o client
