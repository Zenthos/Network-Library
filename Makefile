
all:
	g++ server.cpp libs/Network.cpp -o server.exe -Wall -lws2_32
	g++ client.cpp libs/Network.cpp -o client.exe -Wall -lws2_32

test:
	g++ test.cpp libs/Network.cpp libs/Endpoint.cpp libs/Socket.cpp -o out.exe -Wall -lws2_32

clean:
	rm *.exe