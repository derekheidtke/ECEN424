all: server.C client.C

server: server.C
	g++ -g server.C -o "server"

client: client.C
	g++ client.C -o "client"

mycli: my_client.C
	g++ -g my_client.C -o "myclient"

clean:
	rm "server" "client" "myclient"