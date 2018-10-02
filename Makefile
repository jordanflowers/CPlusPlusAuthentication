all: client server
client: c/client.cpp
	g++ c/client.cpp -o c/client -I /usr/local/ssl/include -L /usr/local/ssl/lib -lssl -lcrypto -Wall
server: s/server.cpp
	g++ s/server.cpp -o s/server -I /usr/local/ssl/include -L /usr/local/ssl/lib -lssl -lcrypto -Wall