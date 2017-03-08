all: server.C client.C

server: server.C
	g++ server.C -o "echos"

client: client.C
	g++ client.C -o "echo"

mycli: my_client.C
	g++ my_client.C -o "myecho"

clean:
	rm "echos" "echo" "myecho"